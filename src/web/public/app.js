const out = document.getElementById("output");

function show(text) {
    out.textContent = text;
}

async function request(url) {
    try {
        const r = await fetch(url, {
            method: "GET",
            cache: "no-store",
            headers: {
                "Accept": "text/plain"
            }
        });
        return await r.text();
    } catch (e) {
        return "Network error: " + e;
    }
}

document.getElementById("helloBtn").onclick = async () => {
    show(await request("/hello"));
};

document.getElementById("infoBtn").onclick = async () => {
    show(await request("/info"));
};

document.getElementById("searchBtn").onclick = async () => {
    const q = document.getElementById("queryInput").value.trim();
    const mode = document.getElementById("modeSelect").value;
    if (!q) return show("Empty query");

    const url = mode === "word"
        ? `/search?q=${encodeURIComponent(q)}`
        : `/search_phrase?q=${encodeURIComponent(q)}`;

    show(await request(url));
};
