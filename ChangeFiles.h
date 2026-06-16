#ifndef CHANGEFILES_H
#define CHANGEFILES_H
#include <QString>
#include <QFile>
#include <QVector>
#include "Definition.h"

void UpdateMesh(QString path){
    QVector<QString> fluFiles;
    QString Grid = "\\ExternalMesh.grid"; fluFiles.push_back(Grid);
    QString Soln = "\\ExternalMesh.soln"; fluFiles.push_back(Soln);
    QString Drop = "\\droplet";
    QString Surf = "\\ExternalMesh.surface"; fluFiles.push_back(Surf);
    QString GFlux = "\\ExternalMesh.hflux"; fluFiles.push_back(GFlux);
    for(int i = 0;i<fluFiles.size();i++){
        QString cht = CHT_PROJ_PATH+fluFiles.at(i);
        if(QFile::exists(cht)){
            QFile::remove(cht);
        }
        QFile::copy(path+"\\FLUENT"+fluFiles.at(i), CHT_PROJ_PATH+fluFiles.at(i));
    }
    if(QFile::exists(CHT_PROJ_PATH+Drop)){
        QFile::remove(CHT_PROJ_PATH+Drop);
    }
    QFile::copy(path+"\\run_DROP3D"+Drop, CHT_PROJ_PATH+Drop);
}

void TurnOnIPS(){
    QString cht_prj = CHT_PROJ_PATH"\\run_CHT3D";
    if(QFile::exists(cht_prj+"\\user_solid.fensap.par")){
        QFile::remove(cht_prj+"\\user_solid.fensap.par");
    }
    QFile::copy(CHT_PARMS_PATH"\\user_solidOn.par", cht_prj+"\\user_solid.fensap.par");
    QFile::setPermissions(cht_prj+"\\user_solid.fensap.par", QFileDevice::ReadOwner| QFileDevice::WriteOwner|QFileDevice::ReadUser);

    if(QFile::exists(cht_prj+"\\user_solid.bc.txt")){
        QFile::remove(cht_prj+"\\user_solid.bc.txt");
    }
    QFile::copy(CHT_PARMS_PATH"\\user_solidOn.txt", cht_prj+"\\user_solid.bc.txt");
     QFile::setPermissions(cht_prj+"\\user_solid.bc.txt", QFileDevice::ReadOwner| QFileDevice::WriteOwner|QFileDevice::ReadUser);
}

void TurnOffIPS(){
    QString cht_prj = CHT_PROJ_PATH"\\run_CHT3D";
    if(QFile::exists(cht_prj+"\\user_solid.fensap.par")){
        QFile::remove(cht_prj+"\\user_solid.fensap.par");
    }
    QFile::copy(CHT_PARMS_PATH"\\user_solidOff.par", cht_prj+"\\user_solid.fensap.par");
    QFile::setPermissions(cht_prj+"\\user_solid.fensap.par", QFileDevice::ReadOwner| QFileDevice::WriteOwner|QFileDevice::ReadUser);

    if(QFile::exists(cht_prj+"\\user_solid.bc.txt")){
        QFile::remove(cht_prj+"\\user_solid.bc.txt");
    }
    QFile::copy(CHT_PARMS_PATH"\\user_solidOff.txt", cht_prj+"\\user_solid.bc.txt");
    QFile::setPermissions(cht_prj+"\\user_solid.bc.txt", QFileDevice::ReadOwner| QFileDevice::WriteOwner|QFileDevice::ReadUser);
}

#endif // CHANGEFILES_H
