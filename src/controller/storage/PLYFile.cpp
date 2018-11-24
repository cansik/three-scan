#include <utility>

#include <utility>

//
// Created by Florian Bruggisser on 24.11.18.
//

#include "PLYFile.h"

PLYFile::PLYFile(PLYFile::SDCardStoragePtr storage) {
    this->sd = storage;
}

void PLYFile::create(String path, int vertexCount) {
    this->vertexCount = vertexCount;

    // write header then vertices
    String header = headerTemplate;
    header.replace("%vertexCount", String(vertexCount));

    file = SD.open(path.c_str(), FILE_WRITE);
    file.print(header);
}

void PLYFile::append(Vertex *vertex) {
    auto position = vertex->getSphericalPosition();
    auto color = vertex->getSignalStrength();

    String m = String(position.x, 4) + " " + String(position.y, 4) + " " + String(position.z, 4)
               + " " + String(color) + " " + String(color) + " " + String(color) + "\n";
    file.print(m);
}

void PLYFile::close() {
    file.flush();
    file.close();
}
