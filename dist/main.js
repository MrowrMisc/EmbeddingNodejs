"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
var fs_1 = __importDefault(require("fs"));
console.log("Hello from Node.js!");
// List the files in the current directory.
fs_1.default.readdirSync(".").forEach(function (file) {
    // Print the file name.
    console.log(file);
});
