const out = document.getElementById("output");

function show(text) {
    out.textContent = text;
}

document.getElementById("helloBtn").onclick = async () => {
    const r = await fetch("/hello");
    show(await r.text());
};

document.getElementById("infoBtn").onclick = async () => {
    const r = await fetch("/info");
    show(await r.text());
};

document.getElementById("searchBtn").onclick = async () => {
    const q = document.getElementById("queryInput").value.trim();
    const mode = document.getElementById("modeSelect").value;
    if (!q) return show("Empty query");

    const url = mode === "word"
        ? `/search?q=${encodeURIComponent(q)}`
        : `/search_phrase?q=${encodeURIComponent(q)}`;

    const r = await fetch(url);
    show(await r.text());
};
