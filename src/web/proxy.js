import express from "express";
import net from "net";

const app = express();
app.use(express.static("public"));

const TCP_HOST = "127.0.0.1";
const TCP_PORT = 8080;

// допоміжна функція: один TCP-запит → одна відповідь
function tcpRequest(message) {
    return new Promise((resolve, reject) => {
        const client = new net.Socket();
        let data = "";

        client.connect(TCP_PORT, TCP_HOST, () => {
            client.write(message + "\n");
        });

        client.on("data", chunk => {
            data += chunk.toString("utf8");
        });

        client.on("end", () => resolve(data));
        client.on("close", () => resolve(data));
        client.on("error", err => reject(err));
    });
}

// HELLO
app.get("/hello", async (req, res) => {
    try {
        const ans = await tcpRequest("HELLO");
        res.send(ans);
    } catch (e) {
        res.status(500).send("Proxy error: " + e.message);
    }
});

// INFO
app.get("/info", async (req, res) => {
    try {
        const ans = await tcpRequest("INFO");
        res.send(ans);
    } catch (e) {
        res.status(500).send("Proxy error: " + e.message);
    }
});

// SEARCH word
app.get("/search", async (req, res) => {
    const q = (req.query.q || "").trim();
    if (!q) return res.status(400).send("Empty query");

    try {
        const ans = await tcpRequest(`SEARCH ${q}`);
        res.send(ans);
    } catch (e) {
        res.status(500).send("Proxy error: " + e.message);
    }
});

// SEARCH_PHRASE phrase
app.get("/search_phrase", async (req, res) => {
    const q = (req.query.q || "").trim();
    if (!q) return res.status(400).send("Empty phrase");

    try {
        const ans = await tcpRequest(`SEARCH_PHRASE ${q}`);
        res.send(ans);
    } catch (e) {
        res.status(500).send("Proxy error: " + e.message);
    }
});

app.listen(3000, () => {
    console.log("[Proxy] HTTP server on http://127.0.0.1:3000");
});
