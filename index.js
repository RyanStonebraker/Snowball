const { app, BrowserWindow } = require('electron');
const path = require('path');
let mainWindow;

app.on('ready', () => {
  mainWindow = new BrowserWindow({
    webPreferences: { nodeIntegration: true },
    resizable: false,
    width: 850,
    height: 625
  });
  mainWindow.loadURL(path.join('file://', __dirname, "index.html"));
});
