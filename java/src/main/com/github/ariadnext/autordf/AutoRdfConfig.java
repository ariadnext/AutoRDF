package com.github.ariadnext.autordf;

import org.bytedeco.javacpp.*;
import org.bytedeco.javacpp.annotation.*;
import org.bytedeco.javacpp.tools.*;

@Properties(
    value = @Platform(
        include = {"Model.h"},
        compiler = "cpp11",
        define = {"SHARED_PTR_NAMESPACE std", "UNIQUE_PTR_NAMESPACE std"}
    ),
    target = "com.github.ariadnext.autordf.AutoRdf"
)
public class AutoRdfConfig implements InfoMapper {
    public void map(InfoMap infoMap) {
        infoMap
        .put(new Info("AUTORDF_EXPORT").cppTypes().annotations())
        .put(new Info("std::list<std::string>").pointerTypes("StringList").define())
        .put(new Info("std::map<std::string,std::string>").pointerTypes("StringMap").define());
    }
}
