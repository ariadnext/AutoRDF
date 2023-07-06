// This file is needed as Meson does not have a generate_export_header equivalent and cannot configure file in a
// specific folder

#ifndef AUTORDF_EXPORT_H
#define AUTORDF_EXPORT_H

#define AUTORDF_EXPORT __attribute__((visibility("default")))
#define AUTORDF_NO_EXPORT __attribute__((visibility("hidden")))

#endif