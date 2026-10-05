import React, { useState } from "react";
import { Link, NavLink } from "react-router-dom";
import { AnimatePresence, motion } from "framer-motion";
import Icon from "./AppIcon";

const NAV = [
  { to: "/", label: "Home", end: true },
  { to: "/lessons", label: "Curriculum" },
  { to: "/lessons/A0.1", label: "First lesson" },
];

const navClass = ({ isActive }) =>
  `rounded-full px-3 py-1.5 text-sm transition ${isActive ? "bg-slate-800 text-white" : "text-slate-300 hover:text-white"}`;

/** Header + footer shared by every page of the academy. */
const SiteShell = ({ children }) => {
  const [open, setOpen] = useState(false);

  return (
    <div className="flex min-h-screen flex-col bg-slate-950 text-slate-100">
      <header className="sticky top-0 z-40 border-b border-slate-800 bg-slate-950/85 backdrop-blur-xl">
        <div className="mx-auto flex max-w-7xl items-center justify-between px-4 py-3.5 sm:px-6 lg:px-8">
          <Link to="/" className="inline-flex items-center gap-2 text-sm font-semibold uppercase tracking-[0.18em] text-cyan-300">
            <img src="/logo.svg" alt="" width="32" height="32" className="h-8 w-8 rounded-lg" />
            <span className="hidden sm:inline">Embedded Pulse Academy</span>
            <span className="sm:hidden">Embedded Pulse</span>
          </Link>

          <nav className="hidden items-center gap-1 md:flex">
            {NAV.map((item) => (
              <NavLink key={item.to} to={item.to} end={item.end} className={navClass}>
                {item.label}
              </NavLink>
            ))}
            <Link to="/lessons" className="ml-2 rounded-full bg-cyan-400 px-4 py-2 text-sm font-semibold text-slate-950 hover:bg-cyan-300">
              Start learning
            </Link>
          </nav>

          <button type="button" onClick={() => setOpen((o) => !o)} className="rounded-lg p-2 text-slate-200 hover:bg-slate-800 md:hidden" aria-label="Menu" aria-expanded={open}>
            <Icon name={open ? "X" : "Menu"} size={20} />
          </button>
        </div>

        <AnimatePresence>
          {open && (
            <motion.nav
              initial={{ height: 0, opacity: 0 }}
              animate={{ height: "auto", opacity: 1 }}
              exit={{ height: 0, opacity: 0 }}
              className="overflow-hidden border-t border-slate-800 md:hidden"
            >
              <div className="flex flex-col gap-1 px-4 py-3">
                {NAV.map((item) => (
                  <NavLink key={item.to} to={item.to} end={item.end} onClick={() => setOpen(false)} className={navClass}>
                    {item.label}
                  </NavLink>
                ))}
              </div>
            </motion.nav>
          )}
        </AnimatePresence>
      </header>

      <div className="flex-1">{children}</div>

      <footer className="border-t border-slate-800 bg-slate-950">
        <div className="mx-auto flex max-w-7xl flex-col gap-3 px-4 py-8 text-sm text-slate-500 sm:flex-row sm:items-center sm:justify-between sm:px-6 lg:px-8">
          <p>Embedded Pulse Academy · STM32 &amp; FreeRTOS from first principles to production.</p>
          <p className="flex items-center gap-2">
            <Icon name="Cpu" size={14} /> Reference board: NUCLEO-F446RE
          </p>
        </div>
      </footer>
    </div>
  );
};

export default SiteShell;
