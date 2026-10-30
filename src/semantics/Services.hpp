#pragma once

class Diagnostics;
class TypeFactory;
class SemanticModel;

struct Services {
    Diagnostics& diagnostics;
    TypeFactory& types;
    SemanticModel& model;

    Services(Diagnostics& diag, TypeFactory& tf, SemanticModel& sm)
        : diagnostics(diag), types(tf), model(sm) {}
};
