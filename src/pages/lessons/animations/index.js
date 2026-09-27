import PromotionComplement from "./PromotionComplement";
import SignedUnsignedCompare from "./SignedUnsignedCompare";
import SignExtension from "./SignExtension";
import TypeWidthExplorer from "./TypeWidthExplorer";
import WrapWheel from "./WrapWheel";

/**
 * Animations usable from lesson Markdown with a fenced block:
 *   ```anim
 *   promotion-complement
 *   ```
 */
const animations = {
  "type-width-explorer": TypeWidthExplorer,
  "promotion-complement": PromotionComplement,
  "signed-unsigned-compare": SignedUnsignedCompare,
  "wrap-wheel": WrapWheel,
  "sign-extension": SignExtension,
};

export default animations;
