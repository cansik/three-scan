#include <utility>

//
// Created by Florian Bruggisser on 24.11.18.
//

#include "PLYFile.h"

PLYFile::PLYFile(PLYFile::SDCardStoragePtr storage) {
    this->sd = storage;
}

void PLYFile::create(String path) {
    this->path = path;
    vertexCount = 0;
}

void PLYFile::append(Vertex *vertex) {
    auto position = vertex->getSphericalPosition();
    auto color = vertex->getSignalStrength();

    String m = String(position.x, 4) + " " + String(position.y, 4) + " " + String(position.z, 4)
               + " " + String(color) + " " + String(color) + " " + String(color) + "\n";

    Serial.print("Line: ");
    Serial.print(m.c_str());

    buffer += m;
    vertexCount++;
}

void PLYFile::close() {
    // write header then vertices
    String header = headerTemplate;
    header.replace("%vertexCount", String(vertexCount));

    sd->writeString(path, header + buffer, true);
}
