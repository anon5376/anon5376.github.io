---
name: qeqx portfolio
description: Infrastructure fixes and reproducible failure cases.
colors:
  paper: "#faf8f3"
  ink: "#252a2d"
  muted: "#596064"
  link: "#174b78"
  rule: "#cbc9c0"
  code: "#efede5"
  good: "#285641"
typography:
  display:
    fontFamily: 'Georgia, "Times New Roman", serif'
    fontSize: "clamp(56px, 8vw, 88px)"
    fontWeight: 400
    lineHeight: 1.05
    letterSpacing: "-.03em"
  headline:
    fontFamily: 'Georgia, "Times New Roman", serif'
    fontSize: "clamp(30px, 4vw, 38px)"
    fontWeight: 400
    lineHeight: 1.2
    letterSpacing: "-.02em"
  body:
    fontFamily: '-apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif'
    fontSize: "17px"
    fontWeight: 400
    lineHeight: 1.6
  label:
    fontFamily: '-apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif'
    fontSize: "14px"
    fontWeight: 400
    lineHeight: 1.6
spacing:
  sm: "8px"
  md: "12px"
  lg: "16px"
  xl: "20px"
  2xl: "24px"
  3xl: "28px"
  4xl: "36px"
  section: "48px"
---

# Design System: qeqx portfolio

## Overview

**Creative North Star: "The Reproducible Record"**

The page presents infrastructure work as a record that can be checked: a short introduction leads to merged and open submissions, then to pinned failure cases with expected, before, and fixed results and links to recipes and logs. Copy stays brief and concrete.

The visual treatment is quiet and document-like. Warm paper, serif headings, restrained rules, and a single blue link color keep attention on the work and its evidence. The page uses system fonts and plain HTML and CSS, with no image or font assets.

**Key Characteristics:**
- Evidence and status are shown as separate, labeled facts.
- Work uses aligned rows and thin rules instead of decorative containers.
- The page collapses its two-column records into one column on narrow screens.

## Colors

Warm paper and dark neutral text form the base; blue identifies links, while green is reserved for successful or merged states.

### Primary
- **Record Blue**: Links and text selection use the sole accent color.

### Neutral
- **Warm Paper**: Page background and favicon ground.
- **Charcoal Ink**: Main text and hover text.
- **Quiet Gray**: Secondary descriptions, dates, notes, and table labels.
- **Rule Gray**: Section and row dividers.
- **Code Sand**: Inline code backgrounds.
- **Result Green**: Merged dates and fixed-result values.

## Typography

**Display Font:** Georgia (with Times New Roman, serif fallback)
**Body Font:** system sans-serif (`-apple-system`, BlinkMacSystemFont, Segoe UI)
**Label/Mono Font:** SFMono-Regular, Consolas, Liberation Mono

**Character:** Serif display headings contrast with compact, readable sans-serif records. Monospace appears only in code and measured technical values.

### Hierarchy
- **Display** (400, fluid 56–88px, 1.05 line-height): the qeqx wordmark.
- **Headline** (400, fluid 30–38px, 1.2 line-height): section titles.
- **Title** (600, 19px, 1.4 line-height): contribution and case names.
- **Body** (400, 17px, 1.6 line-height): default copy; detail paragraphs cap at 65ch.
- **Label** (400, 14px): navigation, dates, checks, notes, and secondary metadata.

## Layout

The centered page is at most 1080px wide, with 64px total side allowance on desktop (32px each) and 40px on screens at or below 720px. Header navigation sits opposite the page descriptor. Content records use a 240px label column, a 36px gap, and a detail column. The introduction has generous vertical space; sections and records are separated by one-pixel rules.

At 720px and below, the page narrows, the header descriptor hides, and the section, contribution, and case grids become one column. The header, intro, section, row, and footer spacing also tighten. Print removes navigation and the skip link, uses a white background, and avoids breaking a case or work row across pages.

## Elevation & Depth

The page is flat: no shadows or raised surfaces. Thin neutral rules separate records; the paper and code fills provide the only tonal surfaces.

## Shapes

There are no rounded containers or card silhouettes. Content remains open on the page, with square edges and thin horizontal separators. Inline code uses a small rectangular background pad.

## Components

### Navigation
- **Style:** Inline blue links with a 14px label size and flexible wrapping. Links have extra vertical padding for a larger target.
- **Hover / Focus:** Hover changes the link to ink and thickens its underline. Keyboard focus uses a 2px blue outline with a 5px offset. Navigation is hidden in print.

### Work Rows
- **Structure:** A contribution name and its detail occupy aligned 240px and flexible columns with a 36px gap.
- **State:** The Merged section is empty and has no merged-date row. Open work uses ruled list rows; the first row has no top rule.
- **Narrow screens:** Name and detail stack with a 12px gap.

### Failure Cases
- **Structure:** Each case pairs a title with a detail column containing a concise reproduction summary, expected/before/fixed result table, and recipe/log links.
- **State:** Cases are divided by thin rules. Result labels use muted text; the fixed value is green.
- **Narrow screens:** Title and evidence stack; links wrap across lines.

## Do's and Don'ts

### Do:
- **Do** keep merged and open contributions in separate sections.
- **Do** label expected, before, and fixed outcomes and link to the recipe and receipts.
- **Do** preserve the one-column record layout at the 720px breakpoint.

### Don't:
- **Don't** present open submissions as accepted work.
- **Don't** add image or font assets; the page uses text and system fonts.
- **Don't** add shadows, rounded cards, or decorative icons to the flat record layout.
