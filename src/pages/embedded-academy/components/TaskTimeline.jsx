import React, { useMemo } from "react";

const TaskTimeline = ({ tasks = [], activeIndex = 0 }) => {
  const safeTasks = useMemo(() => tasks.length ? tasks : ["Task 1", "Task 2", "Task 3"], [tasks]);

  return (
    <div className="rounded-2xl border border-border bg-white p-5 shadow-sm">
      <div className="mb-4 flex items-center justify-between">
        <h3 className="text-lg font-bold text-text-primary">Task scheduler simulation</h3>
        <span className="rounded-full bg-brand-surface px-2.5 py-1 text-xs font-semibold text-brand-primary">Live</span>
      </div>

      <div className="space-y-4">
        {safeTasks.map((task, index) => {
          const isActive = index === activeIndex;
          const isPast = index < activeIndex;

          return (
            <div key={task} className="flex items-center gap-3">
              <div className={`flex h-8 w-8 items-center justify-center rounded-full text-xs font-bold ${
                isActive ? "bg-brand-primary text-white" : isPast ? "bg-emerald-500 text-white" : "bg-slate-200 text-slate-600"
              }`}>
                {index + 1}
              </div>
              <div className="flex-1 rounded-xl border border-border bg-slate-50 px-3 py-2 text-sm text-text-primary">
                {task}
              </div>
            </div>
          );
        })}
      </div>
    </div>
  );
};

export default TaskTimeline;
