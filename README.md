## Arduino and Raspberry Pi Setup

[Full project can be found on Printables](https://www.printables.com/model/1272698-the-big-dawg-feeder)

### Prerequisites

- A **Debian-based** operating system (can be **Ubuntu** but cannot be **Diet-Pi**) is installed on your Raspberry Pi with SSH set-up and a network connection.
- **Arduino IDE** is installed and has a configured environment for the **Arduino Uno R4 Wi-Fi**.
- You have downloaded the **Arduino code** and the **flows.json** file from the GitHub Repository
- Basic knowledge of **Linux** and **Arduino**
- You have wired the components according to the Wiring Diagram

---

### Step 1.1 - Installing Node-RED and Mosquito MQTT on Raspberry Pi

1. Open a new terminal window and connect to your Raspberry Pi.
2. Run this command (you must not be root) to install **Node-RED and Mosquito MQTT** onto your Pi (the contents of the install script is available [here](https://github.com/Arduino-Pro/The-Big-Dawg-Feeder/blob/main/install.sh): 

```powershell
bash <(curl -sL https://raw.githubusercontent.com/Arduino-Pro/TheBigDawgFeeder/main/install.sh)
```

3. Next, you’ll be asked: “Would you like to install Pi-specific nodes?” Press **Y** and **Enter**.

***If you encounter any issues with installation, please refer to the Node-RED [documentation](https://nodered.org/docs/getting-started/raspberrypi).***

### Step 1.2 - Configuring Node-RED on Raspberry Pi

- When prompted to create a settings file, press **Enter**
- Do you want to set up user security? **Yes**.
- Enter a username and press **Enter** (remember it).
- Enter a password and press **Enter** (remember it).
- Enter the password again and press **Enter** (remember it).
- Now define user permissions. Highlight the **Full Access** option in blue and press **Enter**.
- Do you want to enable the Projects feature? **Yes.**
- Choose the **manual** option and press **Enter.**
- Select a theme for the editor. Press **Enter** for **default.**
- Press **Enter** again to select the default text editor.
- Allow Function nodes to load external modules? **Yes**.

If you have made it this far then your configuration was a success. All settings are saved to the file `settings.js`.

Node-RED will start automatically in the background on boot.

### Step 1.3 - Setting up the Dog Feeder Dashboard

To setup your dashboard, open a new browser window and type in your Raspberry Pi IP address followed by `:1880` Example:

```plaintext
10.10.10.10:1880
```

To see your Pi IP, run the following command: 

```powershell
hostname -I
```

After entering the Raspberry Pi IP address followed by :1880 on your web browser, you will see the Node-RED login page. Log in with the username and password you set previously.

Once you have logged in, you should be prompted to Create a Project, Clone a Repository or Open existing project

![](https://static-file-service.macro.com/file/031a99f0-ffe2-41d6-ba38-76fd0e7448c6)

Select **Clone Repository**

You should now see this window:

![](https://static-file-service.macro.com/file/35da37ba-2c93-4f26-8669-5489ea8fe400)

Enter a username and your email address, then press **Next**

Choose a **Project name** and where it says repository, put this in: [https://github.com/Arduino-Pro/TheBigDawgFeeder](https://github.com/Arduino-Pro/TheBigDawgFeeder)

Now press clone repository at the bottom right.

To configure your MQTT out nodes, double click on one of them and add a new MQTT server

![](https://static-file-service.macro.com/file/05b5aa78-b02b-4e82-9167-e53bfab5fdc3)

![](https://static-file-service.macro.com/file/25c4a7ae-460d-41f3-bff5-5e6f46d39c97)

Enter your Raspberry Pi IP address in the **Server** field, enter port 1883 and pick a name for your server. 

Press **Add**

**Make sure that every MQTT node has your server configured**

### Step 1.4 - Deploy

Your project should be ready to deploy! Press the red **Deploy** button at the top right of your screen. Access the UI at`:`

```plaintext
http://Your Pi IP Address:1880/ui
```

If you get an error message saying that some node are not configured properly or if your **Deploy** button is greyed out then just double click on any nodes that have a red triangle on them and then press the **Done** button at the bottom right. If there are no such nodes, then just move one of the nodes.

![](https://static-file-service.macro.com/file/0ba50b23-d6d5-4b5d-9117-f037bc87b08e)

Repeat this for every node with a red triangle. Now try **Deploying** your project again.

**Watch the project video to learn how to use the web interface**

### Congratulations! You have configured your Raspberry Pi!

---

### Step 2.1 - Configure the Sketch for your Arduino

Download the sketch from the GitHub page and install all required libraries

```cpp
#include <WiFiS3.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include <AccelStepper.h>
```

Head over to the secrets tab and fill in your **SSID**, **PASSWORD** and **Raspberry Pi IP**

### Step 2.2 - Upload

Upload the sketch to the Arduino

---
