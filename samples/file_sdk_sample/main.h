#ifndef MAIN_H
#define MAIN_H

#include <string>

void GetLabels(std::string filePath);
void SetLabels(std::string filePath, std::string labelId, std::string owner, std::string justificationMessage);
void ListLabels();
std::string GetExtenstion(const std::string& filePath);

#endif // MAIN_H
