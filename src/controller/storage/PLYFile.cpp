#include <utility>

#include <utility>

//
// Created by Florian Bruggisser on 24.11.18.
//

#include "PLYFile.h"

PLYFile::PLYFile(PLYFile::SDCardStoragePtr storage) {
    this->sd = storage;
}

void PLYFile::create(String path) {
    this->path = std::move(path);
    vertexCount = 0;
}

void PLYFile::append(Vertex *vertex) {
    auto position = vertex->getSphericalPosition();
    auto color = vertex->getSignalStrength();

    String m = String(position.x, 4) + " " + String(position.y, 4) + " " + String(position.z, 4)
               + " " + String(color) + " " + String(color) + " " + String(color) + "\n";

    buffer += m;
    vertexCount++;
}

void PLYFile::close() {
    //sd->writeString("/juan", "test string", true);

    File fj = SD.open("/hell", FILE_APPEND);
    fj.print("jeljadskf\n");
    fj.close();

    //sd->writeString(path.c_str(), getData().c_str(), true);

    String data = getData();

    File file = SD.open(path.c_str(), FILE_WRITE);
    file.print(data.c_str());
    file.flush();
    file.close();
}

String PLYFile::getData() {
    // write header then vertices
    String header = headerTemplate;
    header.replace("%vertexCount", String(vertexCount));

    return String(header + buffer);
}
