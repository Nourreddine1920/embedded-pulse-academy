import React, { useEffect, useId, useState } from "react";

let mermaidPromise = null;

/** Loads mermaid once, on first use (it is large, so keep it out of the main bundle). */
const getMermaid = () => {
  if (!mermaidPromise) {
    mermaidPromise = import("mermaid").then(({ default: mermaid }) => {
      mermaid.initialize({
        startOnLoad: false,
        theme: "dark",
        securityLevel: "strict",
        fontFamily: "Inter, system-ui, sans-serif",
        themeVariables: {
          background: "#020617",
          primaryColor: "#0f172a",
          primaryBorderColor: "#22d3ee",
          primaryTextColor: "#e2e8f0",
          lineColor: "#64748b",
          secondaryColor: "#1e293b",
          tertiaryColor: "#0b1220",
        },
      });
      return mermaid;
    });
  }
  return mermaidPromise;
};

const Mermaid = ({ chart, caption }) => {
  const rawId = useId();
  const id = `mmd-${rawId.replace(/[^a-zA-Z0-9]/g, "")}`;
  const [svg, setSvg] = useState("");
  const [error, setError] = useState(null);

  useEffect(() => {
    let cancelled = false;
    getMermaid()
      .then((mermaid) => mermaid.render(id, chart))
      .then(({ svg: out }) => !cancelled && setSvg(out))
      .catch((e) => !cancelled && setError(String(e.message || e)));
    return () => {
      cancelled = true;
    };
  }, [chart, id]);

  return (
    <figure className="not-prose my-8 rounded-2xl border border-slate-800 bg-slate-950 p-4">
      {error ? (
        <pre className="overflow-auto text-xs text-rose-300">{error}</pre>
      ) : svg ? (
        <div className="flex justify-center overflow-x-auto [&_svg]:h-auto [&_svg]:max-w-full" dangerouslySetInnerHTML={{ __html: svg }} />
      ) : (
        <div className="h-40 animate-pulse rounded-xl bg-slate-900" />
      )}
      {caption && <figcaption className="mt-3 text-center text-xs text-slate-400">{caption}</figcaption>}
    </figure>
  );
};

export default Mermaid;
