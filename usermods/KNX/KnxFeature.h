#include "wled.h"
#include "GroupObject.h"




// class LightObjects {
//   private:
//     std::map<ObjectFunction,String> functionNames;
//     std::map<ObjectType,String> typeNames;

//     void populateMaps();

//   public:
//     std::vector<GroupObject> objects;

//     KnxGroups (std::initializer_list<ObjectFunction> functions) {
//       for (auto &function : functions) {
//         objects.push_back({function, ObjectType::Input});
//         objects.push_back({function, ObjectType::Output});
//       }

//       populateMaps();
//     }
//     String getFunctionName(const GroupObject& object);
//     String getTypeName(const GroupObject& object);    
//     String getObjectName(const GroupObject& object);
//     GroupObject getObject(const ObjectFunction& function, const ObjectType& type);

//     std::vector<GroupObject>::iterator FindObject(ObjectFunction function, ObjectType type) {
//         return std::find_if(objects.begin(), objects.end(), [function, type](const GroupObject& obj) {
//           return obj.function == function && obj.type == type;
//         });
//     }
// };

// void KnxGroups::populateMaps() {
//   functionNames[ObjectFunction::Switch] = "Switch";
//   functionNames[ObjectFunction::Absolute_Dim] = "Absolute dim";
//   functionNames[ObjectFunction::Relative_Dim] = "Relative dim";
//   functionNames[ObjectFunction::Preset] = "Preset";
//   functionNames[ObjectFunction::Playlist] = "Playlist";
//   functionNames[ObjectFunction::Color_Temperature] = "Color temperature";
//   functionNames[ObjectFunction::Color] = "Color";
//   functionNames[ObjectFunction::Effect] = "Effect";
//   functionNames[ObjectFunction::Speed] = "Speed";
//   functionNames[ObjectFunction::Intensity] = "Intensity";
//   functionNames[ObjectFunction::Palette] = "Palette";

//   typeNames[ObjectType::Input] = "Input";
//   typeNames[ObjectType::Output] = "Output";
// }

// String KnxGroups::getFunctionName(const GroupObject& object) {
//   String name;

//   if ((functionNames.find(object.function) != functionNames.end())) {
//     name = functionNames.at(object.function);
//   }

//   return name;
// }

// String KnxGroups::getTypeName(const GroupObject& object) {
//   String name;

//   if ((typeNames.find(object.type) != typeNames.end())) {
//     name = typeNames.at(object.type);
//   }

//   return name;
// }

// String KnxGroups::getObjectName(const GroupObject& object) {
//   String name = getFunctionName(object) + " " + getTypeName(object);

//   return name;
// }

// GroupObject KnxGroups::getObject(const ObjectFunction& function, const ObjectType& type) {

// }