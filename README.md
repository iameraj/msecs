# M.S.E.C.S – Minimal Simple Entity Component System

**M.S.E.C.S** is a minimalistic Entity Component System (ECS). Its simple, easy to use, and focused on essential features.

---

## Features

This ECS currently supports:

- Creating an ECS world with a defined set of allowed component types  
- Adding components to entities  
- Running systems over entities and components  
- Deleting entities  

---

## Limitations

This ECS intentionally does **not** support:

- Direct referencing of individual entities  
- Reordering or repositioning of entities within storage  

Above limitations are by design and keeps the implementation performant.

---

## Usage 
Everything is in "msecs.hpp" header file, just put the file into your project's include dir.

---

## Roadmap

Planned improvements and additions:

- [x] Use a spare set structure for the main component store  
- [x] Introduce a `DenseWorld` class as a complement to the current `World` class.
