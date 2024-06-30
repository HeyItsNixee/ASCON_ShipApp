#include "FileManipulation.h"

SPtr<MbAssembly> CreateSectionSet(SPtr<MbItem>& assemblyItems) {
    SPtr<MbAssembly> a;
    if (assemblyItems != nullptr) {
        a.assign(new MbAssembly());
        a->AddItem(*assemblyItems);
    }
    return a;
}

SPtr<MbItem> CreateC3DSection(const MbCurve& section) {
    return SPtr<MbItem>{ new MbPlaneInstance(MbPlacement3D::global, section) };
}

bool SaveSectionToFile(const MbCurve& section, const c3d::path_string& filePath) {
    SPtr<MbAssembly> assembly = CreateSectionSet(CreateC3DSection(section));
    SPtr<MbItem> it{ assembly.get() };
    if (c3d::ExportIntoFile(*it, filePath) == 0) {
        return true;
    }
    else
        return false;
}


SPtr<MbCurve> GetSectionFromFile(const c3d::path_string& filePath) {
    SPtr<MbItem> it;
    if (c3d::ImportFromFile(it, filePath) == 0) {
        //getcurve
        auto a = dynamic_cast<MbPlaneInstance*>(it.get());
        if (a != nullptr) {
            const MbPlaneItem* pl = a->GetPlaneItem();
            auto b = const_cast<MbPlaneItem*>(pl);
            MbCurve* curve = dynamic_cast<MbCurve*>(b);
            SPtr<MbCurve> curvee;
            curvee.assign(curve);
            return curvee;
        }
    }
    return nullptr;
}