#ifndef SIMPLEMESH_H
#define SIMPLEMESH_H
#include <QObject>
#include <G4ThreeVector.hh>
#include <QJsonObject>
// File modified by Deepak Samuel on 25 Sep 2019
class SimpleMesh : public QObject
{
    Q_OBJECT
public:
    SimpleMesh(QString type, QString qty, bool chargeParticle, bool neutralParticle, bool pid, QString p_id, bool ke, double min_ke, double max_ke);
    SimpleMesh(QString type, QString qty, QString filter="");
    SimpleMesh(QJsonObject json);
    void createMesh();
    void plotProjection();
    void plotColumn(QString plane, int col);
    void execute(QString command);
    void execute(QStringList commands);
    void writeToFile(QString fileName="");
    void makeUnique();
    QStringList scoringCommandList();
    QStringList plotProjectionCommands();
    QStringList plotColumnCommands(QString plane, int column);
    void setBins(G4ThreeVector bin){
        bins=bin;
        jsonObject["binsx"] = bins.x();
        jsonObject["binsy"] = bins.y();
        jsonObject["binsz"] = bins.z();
    }
    void setDims(QList <double> dim){
        dims=dim;
        int ii=0;
        foreach(double d, dims){
            jsonObject[QString("dim-%1").arg(ii++)] = d;
        }
    }
    void append_scoring_filter(QString filter){scoring_filter.append(filter);  jsonObject["scoring_filter"] = scoring_filter.join(",");}
    void set_scoring_filter(QStringList f){scoring_filter=f; jsonObject["scoring_filter"] = scoring_filter.join(",");}
    void set_scoring_qty(QString qty){scoring_qty=qty; jsonObject["scoring_qty"] = scoring_qty;}
    int get_parameter_count(){if(mesh_type=="Box") return 9; else return 8;}
    QString name(){return mesh_name;}
    QString type(){return mesh_type;}
    G4ThreeVector GetPosition(){return position;}
    void SetPosition(G4ThreeVector p){
        position=p;
        jsonObject["posx"] = position.x();
        jsonObject["posy"] = position.y();
        jsonObject["posz"] = position.z();
    }
    G4ThreeVector GetRotation(){return rotation;}
    void SetRotation(G4ThreeVector r){
        rotation=r;
        jsonObject["rotx"] = rotation.x();
        jsonObject["roty"] = rotation.y();
        jsonObject["rotz"] = rotation.z();
    }
    G4ThreeVector GetBins(){return bins;}
    QString measurement(){return scoring_qty;}
    QString filters(){return scoring_filter.join(",");}
    QStringList dim_names(){if(mesh_type=="Box") return box_dim_names; else return cyl_dim_names;}
    QList <double> get_dims(){return dims;}
    void setDims(int ii, double val){
        dims.replace(ii,val);
        jsonObject[QString("dim-%1").arg(ii)] = val;
    }
    QString get_scoring_string(QString helper){
        int i= scoring_string_helper.indexOf(helper);
        if(i>=0){
            return scoring_strings.at(i);
        }
        else
            return "";
    }
    QString get_scoring_filter(QString helper){
        int i= scoring_filter_helper.indexOf(helper);
        if(i>=0){
            return scoring_filters.at(i);
        }
        else
            return "";
    }
    QString get_output_file_name(){
        return mesh_file_name;
 //       QString s = QString("temp-%1-%2.root").arg(scoring_qty).arg(mesh_name);
 //       return s;
    }

    QStringList GetScoringFilterCommands(bool chargeParticle, bool neutralParticle, bool pid, QString p_id, bool ke, double min_ke, double max_ke);
    QJsonObject jsonobject(){return jsonObject;}



private:
    static int mesh_id; // a running id auto incremented
    G4ThreeVector position, rotation, bins;
    QString mesh_name;
    QString mesh_type;
    QString scoring_qty;
    QStringList scoring_filter;
    QList <double> dims; // in the order as in the dim_names;

    QStringList box_dim_names ={"Length mm", "Breadth mm", "Width mm"};
    QStringList cyl_dim_names ={"Length mm", "Radius mm"};
    QStringList scoring_qtys;
    QStringList scoring_qty_filters;

    QStringList scoring_string_helper = {"Energy deposited", "Population", "Dose deposited", "Number of steps", "Number of collisions", "Number of secondaries", "Number of tracks", "Number of terminated tracks", "Track length", "Cell charge", "Cell flux", "Passage cell current", "Passage track length", "Flat surface current", "Flat surface flux"};
    QStringList scoring_strings = {"energyDeposit", "population", "dose Deposit", "nOfStep", "nOfCollision", "nOfSecondary", "nOfTrack", "nOfTerminatedTrack", "trackLength", "cellCharge", "cellFlux", "passageCellCurrent", "passageTrackLength", "flatSurfaceCurrent", "flatSurfaceFlux"};

    QStringList scoring_filter_helper = {"Charged particle filter", "Neutral particle filter", "Kinetic energy filter", "Particle filter", "Particle with kinetic energy filter"};
    QStringList scoring_filters = {"charged", "neutral", "kineticEnergy", "particle", "particleWithKineticEnergy"};
    QJsonObject jsonObject;

    QString mesh_file_name;
};

#endif // SIMPLEMESH_H
