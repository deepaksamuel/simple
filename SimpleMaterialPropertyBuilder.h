#ifndef SIMPLEMATERIALPROPERTYBUILDER_H
#define SIMPLEMATERIALPROPERTYBUILDER_H

#include <QWidget>
#include <SimpleDatabaseManager.h>
#include <G4Material.hh>

namespace Ui {
class SimpleMaterialPropertyBuilder;
}

class SimpleMaterialPropertyBuilder : public QWidget
{
    Q_OBJECT
    
public:
    static SimpleMaterialPropertyBuilder* instance;
    static SimpleMaterialPropertyBuilder *getInstance() {
        if (!instance)
            instance = new SimpleMaterialPropertyBuilder(nullptr);
        return instance;
    }
    
    ~SimpleMaterialPropertyBuilder();
    void ShowUI();
    void SetDataBaseManager(SimpleDatabaseManager* dbM);
    void SetMaterialProperties(G4Material* mat, QString props);
    
private slots:
    void on_addProperty_clicked();

    void on_add_const_property_clicked();

    void on_done_clicked();

    void on_g4_materials_currentIndexChanged(const QString &arg1);

    void on_g4_const_property_currentTextChanged(const QString &arg1);

    void on_g4_const_prop_value_valueChanged(const QString &arg1);

    void on_g4_property_currentTextChanged(const QString &arg1);

private:
    explicit SimpleMaterialPropertyBuilder(QWidget *parent = nullptr);
    Ui::SimpleMaterialPropertyBuilder *ui;
    SimpleDatabaseManager* dbManager;
    QStringList commands;
};

#endif // SIMPLEMATERIALPROPERTYBUILDER_H
