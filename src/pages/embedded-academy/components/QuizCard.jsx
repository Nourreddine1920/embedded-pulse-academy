import React, { useState } from "react";

const QuizCard = ({ question, options, correctAnswer, explanation }) => {
  const [selected, setSelected] = useState(null);
  const [submitted, setSubmitted] = useState(false);

  const handleAnswer = (option) => {
    if (submitted) return;
    setSelected(option);
    setSubmitted(true);
  };

  const isCorrect = selected === correctAnswer;

  return (
    <div className="rounded-2xl border border-border bg-white p-5 shadow-sm">
      <p className="mb-4 text-base font-semibold text-text-primary">{question}</p>
      <div className="space-y-2">
        {options.map((option) => {
          const isSelected = selected === option;
          const isAnswer = option === correctAnswer;
          let stateClass = "border-border bg-slate-50 text-text-primary";

          if (submitted && isAnswer) stateClass = "border-emerald-300 bg-emerald-50 text-emerald-800";
          if (submitted && isSelected && !isAnswer) stateClass = "border-red-300 bg-red-50 text-red-700";

          return (
            <button
              key={option}
              type="button"
              onClick={() => handleAnswer(option)}
              className={`flex w-full items-center justify-between rounded-xl border px-4 py-3 text-left text-sm transition ${stateClass}`}
            >
              <span>{option}</span>
              {submitted && isAnswer && <span className="font-bold">✓</span>}
            </button>
          );
        })}
      </div>

      {submitted && (
        <div className={`mt-4 rounded-xl border px-3 py-2 text-sm ${isCorrect ? "border-emerald-200 bg-emerald-50 text-emerald-800" : "border-red-200 bg-red-50 text-red-700"}`}>
          {isCorrect ? "Correct. " : "Not quite. "}
          {explanation}
        </div>
      )}
    </div>
  );
};

export default QuizCard;
