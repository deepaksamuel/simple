#include "iROOTTree.h"

IROOTTree::IROOTTree()
{
}

QStringList IROOTTree::GetListofLeaves(TTree *tree)
{
    QStringList leavesList;
    if(tree==NULL)return leavesList;
    int nDim=0; //dimension of the arrays if present
    QChar  fillChar = QLatin1Char('0');
    //tree->ResetBranchAddresses();
    TObjArray * leafarray = tree->GetListOfLeaves();
    TObject *leaf;
    QString s,slstr;
    QString s2;
    QStringList sl;
    int leafcnt=0;
    int dim1,dim2;
    if(leafarray!=NULL){
        for(int ii=0;ii<leafarray->GetEntries();ii++){
            leaf = leafarray->At(ii);
            if(leaf!=NULL){
                leavesList.append(QString("%1").arg(leaf->GetName()));
                s = ((TLeaf*) leaf)->GetBranch()->GetName();
                nDim=s.count("[");
                //**IMPROVE**//
                // allow only for 1d and 2d arrays at the moment
                ((TLeaf*)leaf)->GetLeafCounter(leafcnt);
                if(leafcnt>1){ // its an array
                    //cross check for dimensions from leaf title
                    s2=leaf->GetTitle();
                    if(s2.count("[")==2){  nDim=2;
                        s=s2;
                    }
                    if(nDim==1||nDim==0){ // one dimensional array
                        for(int jj =0; jj<leafcnt; jj++){
                            leavesList.append(QString("%1[%2]").arg(leaf->GetName()).arg(jj,3,10,fillChar));
                        }
                    }
                    else if(nDim==2){
                        // Get the dimensions from the string
                        dim1=0; dim2=0;
                        sl=s.split("[");
                        slstr=sl.at(1);
                        slstr=slstr.remove("]");
                        dim1 = slstr.toInt();
                        slstr=sl.at(2);
                        slstr=slstr.remove("]");
                        dim2 = slstr.toInt();

                        for(int ii =0; ii<dim1; ii++){
                            for(int jj=0;jj<dim2;jj++){
                                leavesList.append(QString("%1[%2][%3]").arg(leaf->GetName()).arg(ii,3,10,fillChar).arg(jj,3,10,fillChar));
                            }
                        }
                    }
                }
            }
        }
        return leavesList;
    }
    return leavesList;
}


QStringList IROOTTree::GetListofBranches(TTree *tree)
{
    QStringList leafList;
    //  ui->branchWidget->clear();
    if(tree!=NULL){
        // QCompleter *completer;
        leafList = GetListofLeaves(tree);

        // if tree has friends
        int nFriends=0;
        TList *l = tree->GetListOfFriends();
        if(l!=NULL){
            int n = l->GetEntries();
            if(n>0){
                for(int ii=0;ii<n;ii++){
                    //mainWindow->ShowMessage(QString("Found friend tree: %1...").arg(l->At(ii)->GetName()),"INFO");
                    TFriendElement * fe = (TFriendElement*)l->At(ii);
                    if(fe!=NULL){
                        TTree *t = fe->GetTree();
                        if(t!=NULL){
                            QStringList friendList = GetListofLeaves(t);
                            leafList.append(friendList);
                            nFriends = nFriends+friendList.count();
                        }
                    }
                }
            }
        }
        //        if(leafList.count()>0){
        //            for(int ii=0;ii<leafList.count();ii++){
        //                QTreeWidgetItem *t2 = new QTreeWidgetItem(ui->branchWidget);
        //                t2->setText(0,leafList.at(ii));
        //                if(ii>=(leafList.count()-nFriends)){
        //                    t2->setTextColor(0,Qt::blue);
        //                }

        //            }
        //            completer = new QCompleter(leafList, this);
        //            ui->plotString->setCompleter(completer);
        //            ui->cutString->setCompleter(completer);

        //        }
        return leafList;
    }
    return leafList;

}

QCompleter* IROOTTree::GetCompleter(QStringList branches)
{
    QCompleter *comp = new QCompleter();
    return comp;
}


QString IROOTTree::GetValue(TTree *tree, QString leaf, int entryNumber) //**IMPROVE FOR HIGHER DIMENSIONS
{
    if(tree==NULL){
        return "-9991";
    }
    if(entryNumber<0){
        qDebug()<<"Invalid entry number:"<<entryNumber;
    }

    QString sls,tsls;
    QByteArray c;
    QStringList sl,tsl;
    TLeaf *Leaf;
    c= leaf.toLatin1();
    int indx=0;
    int dim1,dim2;
    int entryNum = tree->GetEntryNumber(entryNumber);
    Long64_t totalEntries;
    if(tree->GetEventList()){
        totalEntries = tree->GetEventList()->GetN();
        if(entryNum>-1){
            tree->GetEntry(entryNum);
        }
        else{

            qDebug()<<"Invalid entry number:"<<entryNumber;
            return "0";
        }
    }
    else{
        totalEntries = tree->GetEntries();
        if(entryNum>-1 && entryNum<totalEntries){
            tree->GetEntry(entryNum);
        }
        else{

            qDebug()<<"Invalid entry number:"<<entryNumber;
            return "0";
        }
    }

    if(leaf.contains("[")){// array field

        if(leaf.count("[")==1){ // 1 dim
            // get string
            sl=leaf.split("[");
            sls=sl.at(0);
            c=sls.toLatin1();
            sls=sl.at(1);
            sls=sls.remove("]");
            indx = sls.toInt();
        }
        if(leaf.count("[")==2){ // 2 dim

            // get dimensions of the current leaf
            // get string
            sl=leaf.split("[");
            sls=sl.at(0);
            c=sls.toLatin1();
            sls=sl.at(1);
            sls=sls.remove("]");
            dim1 = sls.toInt();
            sls=sl.at(2);
            sls=sls.remove("]");
            dim2 = sls.toInt();

            Leaf = tree->GetLeaf(c.data());
            // get array dimensions from the leaf title
            QString title = Leaf->GetTitle();
            int tdim2=0;
            tsl=title.split("[");
            //            tsls=tsl.at(1);
            //            tsls=tsls.remove("]");
            //            tdim1 = tsls.toInt();
            tsls=tsl.at(2);
            tsls=tsls.remove("]");
            tdim2 = tsls.toInt();

            indx=tdim2*dim1+dim2;
        }
    }
    Leaf = tree->GetLeaf(c.data());


    if(Leaf!=NULL){
        if(Leaf->GetTypeName()==TString("TString")){
            QString value ="ERR";
            TString *s = (TString *)Leaf->GetValuePointer();
            if(s!=NULL)
                value = s->Data();
            //delete s;
            return value;
        }
        else if(Leaf->GetTypeName()==TString("Char_t")){
            Char_t *s = (Char_t *)Leaf->GetValuePointer();
            if(s!=NULL)
                return QString("%1").arg(s);
            else
                return "ERR";
        }
        else
            return QString("%1").arg((Leaf->GetValue(indx)));
    }
    else
        return "-9990"; // error gettting leaf
}

int IROOTTree::FilterTree(TTree* tree, QString filter, QString &error)
{
    if(tree!=NULL){
        error="";
        tree->SetEventList(0);
        if(tree->Draw(">>filter",filter.toLatin1().data())>-1){
            TEventList *filteredlist = (TEventList*)gDirectory->Get("filter");
            if(filteredlist!=NULL){
                tree->SetEventList(0);
                tree->SetEventList(filteredlist);
                //                if(ui->filesWidget->isEnabled()){
                //                    ui->filesWidget->currentItem()->setText(1,QString("%1").arg(filteredlist->GetN()));
                //                }
                error = QString("Tree filtered with string: %1 (Passed entries: %2/%3)").arg(filter).arg(filteredlist->GetN()).arg(tree->GetEntriesFast());


                return filteredlist->GetN();
            }
            else{
                error = (QString("No entries returned by filter: %1").arg(filter));
                return -9999;
            }
        }
        else{
            error = (QString("Error in filter string: %1").arg(filter));
            return -9999;
        }
    }
    error ="Invalid tree!";
    return -9999;

}
