/**
 * Loads lesson Markdown (src/content/lessons/<id>.md) and lab source files
 * (src/content/labs/**) lazily, so each lesson is its own chunk.
 */

const lessonFiles = import.meta.glob("/src/content/lessons/*.md", { query: "?raw", import: "default" });
const labFiles = import.meta.glob("/src/content/labs/**/*.{c,h,s,ld,txt,ioc}", { query: "?raw", import: "default" });

const LESSON_DIR = "/src/content/lessons/";
const LAB_DIR = "/src/content/labs/";

/** Ids of lessons that have a Markdown file. */
export const availableLessonIds = new Set(
  Object.keys(lessonFiles).map((path) => path.slice(LESSON_DIR.length).replace(/\.md$/, ""))
);

/** Parses a scalar or an inline [a, b] list from the frontmatter. */
const parseValue = (raw) => {
  const value = raw.trim();
  if (value.startsWith("[") && value.endsWith("]")) {
    const inner = value.slice(1, -1).trim();
    return inner ? inner.split(",").map((item) => item.trim().replace(/^["']|["']$/g, "")) : [];
  }
  return value.replace(/^["']|["']$/g, "");
};

/**
 * Minimal frontmatter parser: `key: value` and `key: [a, b]` lines only.
 * Lesson files are authored by us, so a full YAML parser is not needed.
 */
export const parseFrontmatter = (text) => {
  const match = /^---\r?\n([\s\S]*?)\r?\n---\r?\n?/.exec(text);
  if (!match) return { meta: {}, body: text };

  const meta = {};
  match[1].split(/\r?\n/).forEach((line) => {
    const sep = line.indexOf(":");
    if (sep <= 0 || line.trim().startsWith("#")) return;
    meta[line.slice(0, sep).trim()] = parseValue(line.slice(sep + 1));
  });
  return { meta, body: text.slice(match[0].length) };
};

export const loadLesson = async (id) => {
  const loader = lessonFiles[`${LESSON_DIR}${id}.md`];
  if (!loader) return null;
  return parseFrontmatter(await loader());
};

/** Loads a lab file by its path relative to src/content/labs/. */
export const loadLabFile = async (path) => {
  const loader = labFiles[`${LAB_DIR}${path}`];
  if (!loader) throw new Error(`Lab file not found: ${path}`);
  return loader();
};
