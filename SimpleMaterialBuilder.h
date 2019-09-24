#ifndef SIMPLEMATERIALBUILDER_H
#define SIMPLEMATERIALBUILDER_H

#include <QWidget>
#include <SimpleDatabaseManager.h>
#include <G4Material.hh>
#include <SimpleMaterialPropertyBuilder.h>

namespace Ui {
class SimpleMaterialBuilder;
}

class SimpleMaterialBuilder : public QWidget
{
    Q_OBJECT

public:     
    static SimpleMaterialBuilder* instance;
    static SimpleMaterialBuilder *getInstance() {
        if (!instance)
            instance = new SimpleMaterialBuilder(nullptr);
        return instance;
    }
    ~SimpleMaterialBuilder();
    void ShowUI();
    void SetBuildForElements(bool state);
    G4Material* GetMaterial(QString name, QString formula);
    QString GetMaterialFormula(QString name);
    void SetDataBaseManager(SimpleDatabaseManager* dbM);


private:
    explicit SimpleMaterialBuilder(QWidget *parent = nullptr);


private slots:
    void on_build_from_elements_toggled(bool checked);

    void on_material_stp_toggled(bool checked);

    void on_add_material_clicked();

    void on_add_element_clicked();

    void on_clear_clicked();

    void on_create_material_clicked();

    void on_material_fraction_valueChanged(double arg1);

    void on_addElement_clicked();

    void on_elementalcomposition_toggled(bool checked);

    void on_frac_mass_toggled(bool checked);

    void on_number_of_atoms_toggled(bool checked);

private:
    Ui::SimpleMaterialBuilder *ui;
    SimpleDatabaseManager* dbManager;
    QStringList mat_names;
    QStringList ele_names;
    QList <double> mat_fractions;
    QList <double> ele_fractions;
    QList <int> n_atoms;
    double totalFrac, total_ele_frac;
};

#endif // SIMPLEMATERIALBUILDER_H
