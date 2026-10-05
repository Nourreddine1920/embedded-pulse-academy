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
// A0.3
import ConstPlacement from "./ConstPlacement";
import OptimizerHoist from "./OptimizerHoist";
import VolatileNotAtomic from "./VolatileNotAtomic";
// A0.4
import IoQualifiers from "./IoQualifiers";
import MemoryMapExplorer from "./MemoryMapExplorer";
import PointerCastSteps from "./PointerCastSteps";
// A0.6
import LinkageMap from "./LinkageMap";
import MacroExpansion from "./MacroExpansion";
import StaticLifetime from "./StaticLifetime";
// A0.5
import BitfieldRmw from "./BitfieldRmw";
import PaddingLayout from "./PaddingLayout";
import StructOverlay from "./StructOverlay";
// A1.1
import CompileByCore from "./CompileByCore";
import CoreFamilyExplorer from "./CoreFamilyExplorer";
// A1.2
import IrqMaskLevels from "./IrqMaskLevels";
import RegisterFile from "./RegisterFile";

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
  // A0.3
  "optimizer-hoist": OptimizerHoist,
  "volatile-not-atomic": VolatileNotAtomic,
  "const-placement": ConstPlacement,
  // A0.4
  "memory-map-explorer": MemoryMapExplorer,
  "pointer-cast-steps": PointerCastSteps,
  "io-qualifiers": IoQualifiers,
  // A0.6
  "macro-expansion": MacroExpansion,
  "static-lifetime": StaticLifetime,
  "linkage-map": LinkageMap,
  // A0.5
  "struct-overlay": StructOverlay,
  "padding-layout": PaddingLayout,
  "bitfield-rmw": BitfieldRmw,
  // A1.1
  "core-family-explorer": CoreFamilyExplorer,
  "compile-by-core": CompileByCore,
  // A1.2
  "register-file": RegisterFile,
  "irq-mask-levels": IrqMaskLevels,
};

export default animations;
