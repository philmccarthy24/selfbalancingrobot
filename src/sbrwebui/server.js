const express = require("express");
const fs = require("fs");

//const filebuffer = fs.readFileSync("db/usda-nnd.sqlite3");
//const db = new sqlite.Database(filebuffer);

const app = express();

app.use(express.json());

app.set("port", process.env.PORT || 3001);

// Express only serves static assets in production
if (process.env.NODE_ENV === "production") {
  app.use(express.static("client/build"));
}

// in-memory stubbed state. wire in gRPC here.
const CONFIG = {
  network: {
    wap: {
      enable: false,
      siid: "sbrnet",
      password: "password",
      domain: "www.sbr.net"
    },
    wifi: {
      siid: "VodaPhone 8fe",
      password: "changeme",
      useDHCP: true,
      staticIP: "192.168.0.10",
      subnet: "255.255.255.0",
      dns: "8.8.8.8"
    }
  },
  motion: {
    enableSB: true,
    sbPID_Kp: 1.0,
    sbPID_Ki: 0.5,
    sbPID_Kd: 0.2,
    velocityLimit: 2.0,
    useClientDeviceAccelerometer: false
  }
};

let leftVelocity=0.0;
let rightVelocity=0.0;

let lastPing = Date.now();

app.get("/api/config", (req, res, next) => {
  res.json(CONFIG);
});
app.get("/api/motion", (req, res, next) => {
  res.json({
    left: leftVelocity,
    right: rightVelocity
    });
});
app.get("/api/ping", (req, res, next) => {
  lastPing = Date.now();
  res.send("PONG!")
});
app.put("/api/motion", (req, res, next) => {
  let limitsExceeded = [];
  if (Math.abs(req.body.left) > CONFIG.motion.velocityLimit) {
    limitsExceeded.push('left');
  }
  if (Math.abs(req.body.right) > CONFIG.motion.velocityLimit) {
    limitsExceeded.push('right');
  }
  if (limitsExceeded.length) {
    res.status(400).send(`${limitsExceeded.join(' and ')} motor set beyond velocity limits of +/- ${CONFIG.motion.velocityLimit}`);
  } else {
    leftVelocity = req.body.left;
    rightVelocity = req.body.right;
    res.send("motion updated");
  }
});

app.listen(app.get("port"), () => {
  console.log(`Find the server at: http://localhost:${app.get("port")}/`); // eslint-disable-line no-console
});
