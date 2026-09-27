import React, { useEffect, useState } from "react";
import { Highlight, themes } from "prism-react-renderer";
import Icon from "../../../components/AppIcon";
import { loadLabFile } from "../lessonLoader";

const LANGUAGE_ALIASES = { h: "c", asm: "text", armasm: "text", sh: "text", bash: "text", console: "text", text: "text", ld: "text", make: "text" };
const LINE_NUMBER_THRESHOLD = 12;

/**
 * Highlighted code block. Either `code` is given, or `file` points to a
 * lab source under src/content/labs/ (the exact file that was compiled).
 */
const CodeView = ({ code: inlineCode, language = "text", title, file }) => {
  const [code, setCode] = useState(inlineCode ?? "");
  const [error, setError] = useState(null);
  const [copied, setCopied] = useState(false);

  useEffect(() => {
    if (!file) return undefined;
    let cancelled = false;
    loadLabFile(file)
      .then((text) => !cancelled && setCode(text.replace(/\s+$/, "")))
      .catch((e) => !cancelled && setError(e.message));
    return () => {
      cancelled = true;
    };
  }, [file]);

  const copy = async () => {
    try {
      await navigator.clipboard.writeText(code);
      setCopied(true);
      window.setTimeout(() => setCopied(false), 1200);
    } catch (e) {
      console.error("Copy failed", e);
    }
  };

  const prismLanguage = LANGUAGE_ALIASES[language] ?? language;
  const heading = title || file || (language !== "text" ? language : "");
  const lineCount = code.split("\n").length;
  const numbered = lineCount >= LINE_NUMBER_THRESHOLD;

  return (
    <div className="not-prose my-6 overflow-hidden rounded-xl border border-slate-800 bg-[#011627] shadow-lg">
      <div className="flex items-center justify-between gap-3 border-b border-slate-800 bg-slate-900 px-4 py-2 text-xs text-slate-300">
        <span className="flex min-w-0 items-center gap-2 font-mono">
          {file && <Icon name="FileCode2" size={13} className="shrink-0 text-cyan-300" />}
          <span className="truncate">{heading}</span>
        </span>
        <button
          type="button"
          onClick={copy}
          className="inline-flex shrink-0 items-center gap-1.5 rounded-md border border-slate-700 bg-slate-800 px-2 py-1 text-[11px] font-medium text-slate-200 transition hover:border-slate-500"
        >
          <Icon name={copied ? "Check" : "Copy"} size={12} />
          {copied ? "Copied" : "Copy"}
        </button>
      </div>
      {error ? (
        <p className="p-4 text-sm text-rose-300">{error}</p>
      ) : (
        <Highlight code={code} language={prismLanguage} theme={themes.nightOwl}>
          {({ tokens, getLineProps, getTokenProps }) => (
            <pre className="max-h-[640px] overflow-auto py-3 text-[13px] leading-6">
              <code>
                {tokens.map((line, i) => (
                  <div key={i} {...getLineProps({ line })} className="px-4 hover:bg-white/[0.03]">
                    {numbered && <span className="mr-4 inline-block w-8 select-none text-right text-slate-600">{i + 1}</span>}
                    {line.map((token, key) => (
                      <span key={key} {...getTokenProps({ token })} />
                    ))}
                  </div>
                ))}
              </code>
            </pre>
          )}
        </Highlight>
      )}
    </div>
  );
};

export default CodeView;
