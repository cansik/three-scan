#include <utility>

//
// Created by Florian Bruggisser on 24.11.18.
//

#include "PLYFile.h"

PLYFile::PLYFile(String path, PLYFile::SDCardStoragePtr storage) {
    this->path = path;
    this->sd = storage;
}

void PLYFile::create() {
    vertexCount = 0;
}

void PLYFile::append(Vertex *vertex) {
    auto position = vertex->getSphericalPosition();
    auto color = vertex->getSignalStrength();
    buffer += String(position.x) + " " + String(position.y) + " " + String(position.z)
              + String(color) + " " + String(color) + " " + String(color) + "\n";
    vertexCount++;
}

void PLYFile::close() {
    // write header then vertices
    String header = headerTemplate;
    header.replace("%vertexCount", String(vertexCount));

    sd->writeString(path, header + buffer, true);
}
