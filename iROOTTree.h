#ifndef IROOTTREE_H
#define IROOTTREE_H

// File modified by Deepak Samuel on 25 Sep 2019
#include <QStringList>
#include "TTree.h"
#include "TLeaf.h"
#include "TBranch.h"
#include "TList.h"
#include "TFriendElement.h"
#include "TDirectory.h"
#include "TEventList.h"
#include "QCompleter"
#include <QDebug>

class IROOTTree
{
public:
    IROOTTree();
    static QStringList GetListofLeaves(TTree *tree);
    static QStringList GetListofBranches(TTree *tree);
    static QCompleter* GetCompleter(QStringList branches);
    static QString     GetValue(TTree *tree, QString leaf, int entryNumber);
    static int         FilterTree(TTree *tree, QString filter, QString &error);
};

#endif // IROOTTREE_H
