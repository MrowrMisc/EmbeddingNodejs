import fs from "fs"

console.log("Hello from Node.js!")

// List the files in the current directory.
fs.readdirSync(".").forEach((file) => {
  // Print the file name.
  console.log(file)
})
