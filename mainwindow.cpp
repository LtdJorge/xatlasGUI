#include "mainwindow.h"
#include "ui_mainwindow.h"

#ifdef USE_MULTITHREADING
#define TINYOBJLOADER_OPT_IMPLEMENTATION
#else
#define TINYOBJLOADER_IMPLEMENTATION
#endif


#include "thirdparty/tiny_obj_loader.h"
#include "thirdparty/xatlas.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

static int PrintFunction(const char *format, ...){

    return 0;
}

extern void AddMeshToAtlas(int index, std::vector<tinyobj::shape_t> shapes, std::vector<tinyobj::material_t> materials,
                           tinyobj::attrib_t attrib){
    const tinyobj::mesh_t &obj_mesh = shapes[index].mesh;
    xatlas::MeshDecl meshDecl;
    //meshDecl.vertexCount = static_cast<uint32_t>(obj_mesh.indices.size() / 3);
    //meshDecl.vertexPositionData = attrib.vertices.;
    size_t index_offset = 0;
    for(size_t f = 0; f < obj_mesh.num_face_vertices.size(); f++){
        size_t fv = obj_mesh.num_face_vertices[f];

        for(size_t v = 0; v < fv; v++){
            tinyobj::index_t idx = obj_mesh.indices[index_offset + v];
            tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index+0];
            tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index+1];
            tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index+2];
            tinyobj::real_t nx = attrib.normals[3*idx.normal_index+0];
            tinyobj::real_t ny = attrib.normals[3*idx.normal_index+1];
            tinyobj::real_t nz = attrib.normals[3*idx.normal_index+2];
            tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
            tinyobj::real_t ty = attrib.texcoords[2*idx.texcoord_index+1];
        }
        index_offset += fv;
    }
    meshDecl.vertexPositionStride = sizeof (float) * 3;
    //meshDecl.vertexNormalData = obj_mesh.indices;
    if(!obj_mesh.indices)
}

void MainWindow::open(){
    if(maybeSave()){

        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn;
        std::string err;
        tinyobj::attrib_t attrib;

        std::string fileName = QFileDialog::getOpenFileName(this).toStdString();
        if(!fileName.empty()){
            if(!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, fileName.c_str())){
                QMessageBox msgBox;
                std::string errorString = "Error: " + err + "\n";
                msgBox.setText(errorString.c_str());
                msgBox.setIcon(QMessageBox::Critical);
                msgBox.exec();
            }
            if(shapes.size() == 0){
                QMessageBox msgBox;
                msgBox.setText("Error: the obj is empty (no shapes).");
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.exec();
            }
            xatlas::SetPrint(PrintFunction,false);

            xatlas::Atlas *atlas = xatlas::Create();
            QFuture<void> *futures = new QFuture<void>[shapes.size()];

            int threadCount = static_cast<int>(shapes.size());
            //if(threadCount>QThread::idealThreadCount()){

            //}
            for (int i =0;i<=threadCount;i++) {
                futures[i] = QtConcurrent::run(AddMeshToAtlas, i, shapes, materials, attrib);
            }

        }
    }
}

void MainWindow::loadFile(const QString &filename){
    QFile file(filename);
    if(!file.open(QFile::ReadOnly | QFile::Text)){

    }
}

bool MainWindow::maybeSave(){
    if(true){
        return true;
    }
}
