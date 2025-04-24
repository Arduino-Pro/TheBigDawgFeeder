#!/bin/bash

set -e

echo "🔧 Updating system packages..."
sudo apt update && sudo apt upgrade -y

echo "🔧 Installing required packages..."
sudo apt install -y build-essential git curl mosquitto mosquitto-clients

echo "🔧 Installing Node.js and Node-RED..."
bash <(curl -sL https://raw.githubusercontent.com/node-red/linux-installers/master/deb/update-nodejs-and-nodered)

echo "🔧 Configuring Mosquitto for anonymous access..."
MOSQUITTO_CONF_DIR="/etc/mosquitto/conf.d"
MOSQUITTO_CONF_FILE="$MOSQUITTO_CONF_DIR/allow_anonymous.conf"
sudo mkdir -p "$MOSQUITTO_CONF_DIR"
echo -e "listener 1883\nallow_anonymous true" | sudo tee "$MOSQUITTO_CONF_FILE" > /dev/null

echo "🔧 Restarting Mosquitto to apply configuration..."
sudo systemctl restart mosquitto

echo "🔧 Installing project dependencies..."
npm install node-red-dashboard node-red-contrib-moment

echo "🔧 Starting Node-RED..."
USER=$(logname)
sudo -u "$USER" bash -c node-red-start &

echo "🔧 Enabling Node-RED to start on boot..."
sudo systemctl enable nodered.service

sudo reboot
