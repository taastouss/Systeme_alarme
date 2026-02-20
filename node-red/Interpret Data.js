let parts = msg.payload;
let type = parts[0];

let out = {
    event: type,
    timestamp: new Date().toISOString()
};

if (type === "ALIVE") {
    out.state = "ALIVE";
}

else if (type === "SOS") {
    out.state = "SOS";
    out.temperature = parts[1] ? Number(parts[1]) : null;
}

else if (type === "FEU") {
    out.state = "FEU";
    out.angle = parts[1] ? Number(parts[1]) : null;
    out.temperature = parts[2] ? Number(parts[2]) : null;
}

else {
    out.state = "UNKNOWN";
}

msg.payload = out;

return msg;
