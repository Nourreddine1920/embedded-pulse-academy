import React, { useState } from "react";
import ReactMarkdown from "react-markdown";
import remarkGfm from "remark-gfm";
import { Link } from "react-router-dom";
import { AnimatePresence, motion } from "framer-motion";
import Icon from "../../../components/AppIcon";
import animations from "../animations";
import CodeView from "./CodeView";
import Mermaid from "./Mermaid";
import QuizBlock from "./QuizBlock";

/** "file=a/b.c title="Main file"" -> { file: "a/b.c", title: "Main file" } */
const parseMeta = (meta = "") => {
  const out = {};
  const re = /(\w+)=(?:"([^"]*)"|(\S+))|(\w+)/g;
  let m;
  while ((m = re.exec(meta)) !== null) {
    if (m[1]) out[m[1]] = m[2] ?? m[3];
    else if (m[4]) out[m[4]] = true;
  }
  return out;
};

export const slugify = (text) =>
  String(text).toLowerCase().replace(/[^a-z0-9]+/g, "-").replace(/(^-|-$)/g, "");

const textOf = (children) =>
  React.Children.toArray(children)
    .map((c) => (typeof c === "string" ? c : c?.props?.children ? textOf(c.props.children) : ""))
    .join("");

/* ---- Fenced-block widgets --------------------------------------------- */

const LAYERS = {
  beginner: { emoji: "🟢", title: "Beginner · What & Why", ring: "border-emerald-500/40", head: "bg-emerald-500/10 text-emerald-200" },
  intermediate: { emoji: "🟡", title: "Intermediate · How to use it", ring: "border-amber-500/40", head: "bg-amber-500/10 text-amber-200" },
  advanced: { emoji: "🔴", title: "Advanced · Under the hood & in production", ring: "border-rose-500/40", head: "bg-rose-500/10 text-rose-200" },
};

const Layer = ({ level, title, children }) => {
  const cfg = LAYERS[level] || LAYERS.beginner;
  return (
    <section className={`not-prose my-8 overflow-hidden rounded-2xl border ${cfg.ring} bg-slate-900/40`}>
      <div className={`flex items-center gap-2 px-5 py-3 text-sm font-bold uppercase tracking-[0.12em] ${cfg.head}`}>
        <span className="text-base">{cfg.emoji}</span>
        {title || cfg.title}
      </div>
      <div className="lesson-prose px-5 pb-2 pt-1">{children}</div>
    </section>
  );
};

const CALLOUTS = {
  tip: { icon: "Lightbulb", cls: "border-cyan-400/40 bg-cyan-500/5", head: "text-cyan-200", label: "Tip" },
  note: { icon: "Info", cls: "border-sky-400/40 bg-sky-500/5", head: "text-sky-200", label: "Note" },
  warning: { icon: "TriangleAlert", cls: "border-amber-400/50 bg-amber-500/5", head: "text-amber-200", label: "Warning" },
  danger: { icon: "OctagonAlert", cls: "border-rose-400/50 bg-rose-500/5", head: "text-rose-200", label: "Danger" },
  analogy: { icon: "Sparkles", cls: "border-violet-400/40 bg-violet-500/5", head: "text-violet-200", label: "Analogy" },
  production: { icon: "Factory", cls: "border-emerald-400/40 bg-emerald-500/5", head: "text-emerald-200", label: "In production" },
};

const Callout = ({ kind, title, children }) => {
  const cfg = CALLOUTS[kind] || CALLOUTS.note;
  return (
    <aside className={`not-prose my-6 rounded-xl border-l-4 ${cfg.cls} px-5 py-4`}>
      <p className={`mb-1 flex items-center gap-2 text-sm font-bold ${cfg.head}`}>
        <Icon name={cfg.icon} size={16} />
        {title || cfg.label}
      </p>
      <div className="lesson-prose lesson-prose-tight">{children}</div>
    </aside>
  );
};

const Solution = ({ title, children }) => {
  const [open, setOpen] = useState(false);
  return (
    <div className="not-prose my-4 overflow-hidden rounded-xl border border-slate-700 bg-slate-900/60">
      <button type="button" onClick={() => setOpen((o) => !o)} className="flex w-full items-center justify-between px-4 py-3 text-left text-sm font-semibold text-slate-200 hover:bg-slate-800/60">
        <span className="flex items-center gap-2">
          <Icon name={open ? "EyeOff" : "Eye"} size={15} className="text-cyan-300" />
          {title || "Show solution"}
        </span>
        <motion.span animate={{ rotate: open ? 180 : 0 }}>
          <Icon name="ChevronDown" size={16} />
        </motion.span>
      </button>
      <AnimatePresence initial={false}>
        {open && (
          <motion.div initial={{ height: 0, opacity: 0 }} animate={{ height: "auto", opacity: 1 }} exit={{ height: 0, opacity: 0 }} transition={{ duration: 0.25 }}>
            <div className="lesson-prose border-t border-slate-800 px-4 pb-2">{children}</div>
          </motion.div>
        )}
      </AnimatePresence>
    </div>
  );
};

/* ---- Markdown element mapping ----------------------------------------- */

const renderFence = (language, meta, text) => {
  const props = parseMeta(meta);
  switch (language) {
    case "anim": {
      const Anim = animations[text.trim()];
      return Anim ? <Anim /> : <p className="text-rose-300">Unknown animation: {text}</p>;
    }
    case "mermaid":
      return <Mermaid chart={text} caption={props.caption} />;
    case "quiz":
      return <QuizBlock source={text} />;
    case "layer":
      return (
        <Layer level={Object.keys(props).find((k) => LAYERS[k])} title={props.title}>
          <MarkdownLesson source={text} />
        </Layer>
      );
    case "callout":
      return (
        <Callout kind={Object.keys(props).find((k) => CALLOUTS[k])} title={props.title}>
          <MarkdownLesson source={text} />
        </Callout>
      );
    case "solution":
      return (
        <Solution title={props.title}>
          <MarkdownLesson source={text} />
        </Solution>
      );
    default:
      return <CodeView code={props.file ? undefined : text.replace(/\n$/, "")} language={language || "text"} title={props.title} file={props.file} />;
  }
};

const components = {
  pre: ({ node }) => {
    const codeNode = node?.children?.[0];
    const className = codeNode?.properties?.className?.[0] || "";
    const language = className.replace(/^language-/, "");
    const text = codeNode?.children?.map((c) => c.value || "").join("") || "";
    return renderFence(language, codeNode?.data?.meta, text);
  },
  code: ({ children }) => (
    <code className="rounded-md border border-slate-700/60 bg-slate-800/80 px-1.5 py-0.5 font-mono text-[0.85em] text-cyan-200 [overflow-wrap:anywhere]">{children}</code>
  ),
  h2: ({ children }) => {
    const id = slugify(textOf(children));
    return (
      <h2 id={id} className="group mt-16 scroll-mt-24 border-t border-slate-800 pt-10 text-2xl font-black tracking-tight text-white sm:text-3xl">
        <a href={`#${id}`} className="no-underline">
          {children}
          <span className="ml-2 text-cyan-500 opacity-0 transition group-hover:opacity-100">#</span>
        </a>
      </h2>
    );
  },
  h3: ({ children }) => {
    const id = slugify(textOf(children));
    return (
      <h3 id={id} className="mt-10 scroll-mt-24 text-xl font-bold text-white">
        {children}
      </h3>
    );
  },
  h4: ({ children }) => <h4 className="mt-7 text-base font-bold uppercase tracking-[0.08em] text-cyan-200">{children}</h4>,
  p: ({ children }) => <p className="my-4 text-[16px] leading-8 text-slate-300">{children}</p>,
  ul: ({ children }) => <ul className="my-4 list-disc space-y-2 pl-6 text-[16px] leading-7 text-slate-300 marker:text-cyan-400">{children}</ul>,
  ol: ({ children }) => <ol className="my-4 list-decimal space-y-2 pl-6 text-[16px] leading-7 text-slate-300 marker:font-bold marker:text-cyan-400">{children}</ol>,
  li: ({ children }) => <li className="pl-1">{children}</li>,
  strong: ({ children }) => <strong className="font-semibold text-white">{children}</strong>,
  blockquote: ({ children }) => <blockquote className="my-6 border-l-4 border-cyan-500/50 bg-slate-900/60 px-5 py-1 italic">{children}</blockquote>,
  hr: () => <hr className="my-10 border-slate-800" />,
  a: ({ href = "", children }) =>
    href.startsWith("/") ? (
      <Link to={href} className="font-medium text-cyan-300 underline decoration-cyan-500/40 underline-offset-4 hover:decoration-cyan-300">
        {children}
      </Link>
    ) : (
      <a href={href} target={href.startsWith("#") ? undefined : "_blank"} rel="noreferrer" className="font-medium text-cyan-300 underline decoration-cyan-500/40 underline-offset-4 hover:decoration-cyan-300">
        {children}
      </a>
    ),
  table: ({ children }) => (
    <div className="not-prose my-6 overflow-x-auto rounded-xl border border-slate-800">
      <table className="w-full border-collapse text-left text-sm">{children}</table>
    </div>
  ),
  thead: ({ children }) => <thead className="bg-slate-900 text-xs uppercase tracking-[0.08em] text-cyan-200">{children}</thead>,
  th: ({ children }) => <th className="whitespace-nowrap border-b border-slate-800 px-4 py-3 font-semibold">{children}</th>,
  td: ({ children }) => <td className="border-b border-slate-800/70 px-4 py-3 align-top leading-6 text-slate-300">{children}</td>,
  tr: ({ children }) => <tr className="even:bg-slate-900/30">{children}</tr>,
};

/** Renders lesson Markdown with the Embedded Pulse widgets. */
const MarkdownLesson = ({ source }) => (
  <ReactMarkdown remarkPlugins={[remarkGfm]} components={components}>
    {source}
  </ReactMarkdown>
);

export default MarkdownLesson;
