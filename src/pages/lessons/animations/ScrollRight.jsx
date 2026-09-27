import React, { useEffect, useRef } from "react";

/**
 * Horizontal scroll area that starts (and stays) scrolled to the right edge,
 * so on narrow screens bit rows show their low bits (bit 0) first.
 */
const ScrollRight = ({ className = "", children }) => {
  const ref = useRef(null);

  useEffect(() => {
    const el = ref.current;
    if (el) el.scrollLeft = el.scrollWidth;
  });

  return (
    <div ref={ref} className={`overflow-x-auto ${className}`}>
      {children}
    </div>
  );
};

export default ScrollRight;
