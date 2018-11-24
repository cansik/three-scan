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

    file.print(position.x);
    file.print(" ");
    file.print(position.y);
    file.print(" ");
    file.print(position.z);
    file.print(" ");
    file.print(color);
    file.print(" ");
    file.print(color);
    file.print(" ");
    file.print(color);
    file.print("\n");
}

void PLYFile::close() {
    // wait for print
    for (int i = 0; i < 500; i++) {
        delay(2);
    }

    //file.println("the end");

    file.flush();
    file.close();
}
