// Scene.cpp
#include "Scene.h"
#include "tinyxml2.h"
#include <iostream>
#include <sstream>

using namespace tinyxml2;

Scene::Scene() : mapLoaded(false), tilesetTexture({ 0 }) {}

Scene::~Scene() {
    Unload();
}

bool Scene::LoadMap(const std::string& mapPath) {
    XMLDocument doc;
    if (doc.LoadFile(mapPath.c_str()) != XML_SUCCESS) {
        std::cerr << "Failed to load map: " << mapPath << " | Error: " << doc.ErrorStr() << std::endl;
        return false;
    }

    XMLElement* mapElement = doc.FirstChildElement("map");
    if (!mapElement) {
        std::cerr << "Invalid TMX file: No <map> element found." << std::endl;
        return false;
    }

    XMLElement* tilesetElement = mapElement->FirstChildElement("tileset");
    if (!tilesetElement) {
        std::cerr << "Invalid TMX file: No <tileset> element found." << std::endl;
        return false;
    }

    XMLElement* imageElement = tilesetElement->FirstChildElement("image");
    if (!imageElement) {
        std::cerr << "Invalid TMX file: No <image> element in tileset." << std::endl;
        return false;
    }

    const char* tilesetPath = imageElement->Attribute("source");
    if (!tilesetPath) {
        std::cerr << "Tileset image source not found." << std::endl;
        return false;
    }

    tilesetTexture = LoadTexture(tilesetPath);
    if (tilesetTexture.id == 0) {
        std::cerr << "Failed to load tileset texture: " << tilesetPath << std::endl;
        return false;
    }

    XMLElement* layerElement = mapElement->FirstChildElement("layer");
    while (layerElement) {
        XMLElement* dataElement = layerElement->FirstChildElement("data");
        if (dataElement) {
            const char* tileData = dataElement->GetText();
            if (tileData) {
                std::istringstream tileStream(tileData);
                int tileID;
                int x = 0, y = 0;

                while (tileStream >> tileID) {
                    if (tileID > 0) {
                        Tile tile;
                        tile.id = tileID;
                        tile.position = { x * 32.0f, y * 32.0f };
                        tile.sourceRect = { (float)((tileID - 1) % (tilesetTexture.width / 32)) * 32,
                                            (float)((tileID - 1) / (tilesetTexture.width / 32)) * 32,
                                            32.0f, 32.0f };
                        tiles.push_back(tile);
                    }

                    if (tileStream.peek() == ',') tileStream.ignore();

                    x++;
                    if (x >= 10) { // Adjust based on your map's width
                        x = 0;
                        y++;
                    }
                }
            }
        }
        layerElement = layerElement->NextSiblingElement("layer");
    }

    mapLoaded = true;
    return true;
}

void Scene::Render() {
    if (mapLoaded) {
        for (const Tile& tile : tiles) {
            DrawTextureRec(tilesetTexture, tile.sourceRect, tile.position, WHITE);
        }
    }
    else {
        DrawText("Map not loaded!", 10, 10, 20, RED);
    }
}

void Scene::Update() {
    // Actualizar lógica del mapa aquí
}

void Scene::Unload() {
    if (mapLoaded) {
        UnloadTexture(tilesetTexture);
        tiles.clear();
        mapLoaded = false;
    }
}