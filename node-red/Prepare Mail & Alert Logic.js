let data = msg.payload;
let now = Date.now();


// =======================
// 🔴 GESTION SOS
// =======================

if (data.state === "SOS") {

    let sosSent = context.get("sosSent") || false;

    // Si déjà envoyé → on bloque
    if (sosSent) {
        return null;
    }

    // Sinon on envoie UNE fois
    context.set("sosSent", true);

    msg.topic = "🚨 ALERTE SOS";
    msg.payload =
        "⚠️ ALERTE SOS ⚠️\n\n" +
        "Date : " + data.timestamp + "\n" +
        (data.temperature != null ? "🌡 Température : " + data.temperature + " °C\n" : "") +
        "\n---\nFireScan System";

    return msg;
}


// Reset du SOS si l'état change
if (data.state !== "SOS") {
    context.set("sosSent", false);
}


// =======================
// 🔥 GESTION FEU
// =======================

if (data.state === "FEU" && data.temperature != null) {

    let seuil = 32;      // seuil température
    let duree = 3000;    // 3 secondes

    let startTime = context.get("feuStartTime");

    if (data.temperature > seuil) {

        if (!startTime) {
            context.set("feuStartTime", now);
            return null;
        }

        if (now - startTime >= duree) {

            // Reset pour éviter spam
            context.set("feuStartTime", null);

            msg.topic = "🔥 ALERTE FEU CONFIRMÉE";
            msg.payload =
                "🔥 FEU DÉTECTÉ 🔥\n\n" +
                "Température > " + seuil + " °C pendant 3 secondes\n\n" +
                "🌡 Température : " + data.temperature + " °C\n" +
                (data.angle != null ? "🎯 Angle servo : " + data.angle + " °\n" : "") +
                "Date : " + data.timestamp + "\n\n" +
                "---\nFireScan System";

            return msg;
        }

        return null;
    }

    // Si la température redescend → reset
    context.set("feuStartTime", null);
    return null;
}

return null;
