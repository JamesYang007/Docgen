#!/usr/bin/env node

const fs = require('fs');
const json2html = require('node-json2html');

const jsonData = JSON.parse(fs.readFileSync(0, 'utf-8'));

// JSON processing to HTML here
var htmlSnippet = json2html.transform(jsonData,{'<>':'li','html':'${name}'});

fs.writeFileSync(1, htmlSnippet);
