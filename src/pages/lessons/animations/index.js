import BitOpsPlayground from "./BitOpsPlayground";
import FieldRmw from "./FieldRmw";
import FlagClear from "./FlagClear";
import PromotionComplement from "./PromotionComplement";
import RmwRace from "./RmwRace";
import ShiftLab from "./ShiftLab";
import SignedUnsignedCompare from "./SignedUnsignedCompare";
import SignExtension from "./SignExtension";
import TypeWidthExplorer from "./TypeWidthExplorer";
import WrapWheel from "./WrapWheel";
import rtosAnimations from "./rtos";

/**
 * Animations usable from lesson Markdown with a fenced block:
 *   ```anim
 *   promotion-complement
 *   ```
 */
const animations = {
  // Part B (FreeRTOS): registered per module in ./rtos, keys prefixed "rtos-"
  ...rtosAnimations,
  "type-width-explorer": TypeWidthExplorer,
  "promotion-complement": PromotionComplement,
  "signed-unsigned-compare": SignedUnsignedCompare,
  "wrap-wheel": WrapWheel,
  "sign-extension": SignExtension,
  // A0.2
  "bit-ops-playground": BitOpsPlayground,
  "field-rmw": FieldRmw,
  "rmw-race": RmwRace,
  "flag-clear": FlagClear,
  "shift-lab": ShiftLab,
};

export default animations;
