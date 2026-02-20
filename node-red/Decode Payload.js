// Décodage Base64 -> texte ASCII
let txt = Buffer.from(msg.payload.frmPayload, 'base64')
              .toString('utf8')
              .trim();

// Découpe par virgule
let parts = txt.split(",");

// Exemples reçus :
// ["ALIVE"]
// ["SOS","22.1"]
// ["FEU","153","40.6"]

msg.payload = parts;

return msg;
