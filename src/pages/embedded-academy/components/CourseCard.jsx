import React from "react";
import Icon from "../../../components/AppIcon";

const CourseCard = ({
  title,
  description,
  difficulty,
  duration,
  level,
  tags = [],
  accent = "brand-primary",
  onSelect,
}) => {
  const palette = {
    "brand-primary": "bg-brand-primary text-white",
    "brand-secondary": "bg-brand-secondary text-white",
    "brand-accent": "bg-cyan-500 text-white",
    success: "bg-emerald-500 text-white",
    warning: "bg-amber-500 text-white",
  };

  const badgeStyles = {
    Beginner: "bg-emerald-100 text-emerald-700 border border-emerald-200",
    Intermediate: "bg-blue-100 text-blue-700 border border-blue-200",
    Advanced: "bg-violet-100 text-violet-700 border border-violet-200",
  };

  return (
    <article className="group h-full rounded-2xl border border-border bg-white p-5 shadow-sm transition-all duration-200 hover:-translate-y-1 hover:shadow-xl">
      <div className="mb-4 flex items-start justify-between gap-3">
        <div className={`flex h-11 w-11 items-center justify-center rounded-xl ${palette[accent] || palette["brand-primary"]}`}>
          <Icon name="BookOpen" size={18} />
        </div>
        <span className={`rounded-full px-2.5 py-1 text-xs font-semibold ${badgeStyles[difficulty] || "bg-slate-100 text-slate-700"}`}>
          {difficulty}
        </span>
      </div>

      <div className="mb-3 flex items-center gap-2 text-xs font-medium uppercase tracking-[0.12em] text-text-secondary">
        <span>{level}</span>
        <span>•</span>
        <span>{duration}</span>
      </div>

      <h3 className="mb-3 text-xl font-bold text-text-primary">{title}</h3>
      <p className="mb-4 text-sm leading-6 text-text-secondary">{description}</p>

      <div className="mb-5 flex flex-wrap gap-2">
        {tags.map((tag) => (
          <span key={tag} className="rounded-full bg-brand-surface px-2.5 py-1 text-[11px] font-medium text-text-secondary">
            {tag}
          </span>
        ))}
      </div>

      <button
        type="button"
        onClick={onSelect}
        className="inline-flex items-center gap-2 rounded-lg bg-brand-primary px-4 py-2 text-sm font-semibold text-white transition hover:bg-brand-secondary"
      >
        Explore path
        <Icon name="ArrowRight" size={15} />
      </button>
    </article>
  );
};

export default CourseCard;
