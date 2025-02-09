"use strict";

module.exports = { // eslint-disable-line no-undef
  "extends": [
    "../../.eslintrc.js"
  ],

  "globals": {
    "Components": true,
    "dump": true,
    "Iterator": true
  },

  "env": { "browser": true },

  "rules": {
    // Mozilla stuff
    "mozilla/no-aArgs": "warn",
    "mozilla/reject-importGlobalProperties": "warn",
    "mozilla/var-only-at-top-level": "warn",

    "block-scoped-var": "error",
    "brace-style": ["warn", "1tbs", {"allowSingleLine": false}],
    "camelcase": "warn",
    "comma-dangle": "off",
    "comma-spacing": ["warn", {"before": false, "after": true}],
    "comma-style": ["warn", "last"],
    "complexity": ["error", {"max": 20}],
    "consistent-return": "error",
    "curly": "error",
    "dot-location": ["warn", "property"],
    "dot-notation": "error",
    "eol-last": "error",
    "generator-star-spacing": ["warn", "after"],
    "indent": ["warn", 2, {"SwitchCase": 1}],
    "key-spacing": ["warn", {"beforeColon": false, "afterColon": true}],
    "keyword-spacing": "warn",
    "max-len": ["warn", 80, 2, {"ignoreUrls": true}],
    "max-nested-callbacks": ["error", 3],
    "new-cap": ["error", {"capIsNew": false}],
    "new-parens": "error",
    "no-array-constructor": "error",
    "no-cond-assign": "error",
    "no-control-regex": "error",
    "no-debugger": "error",
    "no-delete-var": "error",
    "no-dupe-args": "error",
    "no-dupe-keys": "error",
    "no-duplicate-case": "error",
    "no-else-return": "error",
    "no-eval": "error",
    "no-extend-native": "error",
    "no-extra-bind": "error",
    "no-extra-boolean-cast": "error",
    "no-extra-semi": "warn",
    "no-fallthrough": "error",
    "no-inline-comments": "warn",
    "no-lonely-if": "error",
    "no-mixed-spaces-and-tabs": "error",
    "no-multi-spaces": "warn",
    "no-multi-str": "warn",
    "no-multiple-empty-lines": ["warn", {"max": 1}],
    "no-native-reassign": "error",
    "no-nested-ternary": "error",
    "no-redeclare": "error",
    "no-return-assign": "error",
    "no-self-compare": "error",
    "no-sequences": "error",
    "no-shadow": "warn",
    "no-shadow-restricted-names": "error",
    "no-spaced-func": "warn",
    "no-throw-literal": "error",
    "no-trailing-spaces": "error",
    "no-undef": "error",
    "no-unneeded-ternary": "error",
    "no-unreachable": "error",
    "no-unused-vars": "error",
    "no-with": "error",
    "padded-blocks": ["warn", "never"],
    "quotes": ["warn", "double", "avoid-escape"],
    "semi": ["warn", "always"],
    "semi-spacing": ["warn", {"before": false, "after": true}],
    "space-before-blocks": ["warn", "always"],
    "space-before-function-paren": ["warn", "never"],
    "space-in-parens": ["warn", "never"],
    "space-infix-ops": ["warn", {"int32Hint": true}],
    "space-unary-ops": ["warn", { "words": true, "nonwords": false }],
    "spaced-comment": ["warn", "always"],
    "strict": ["error", "global"],
    "use-isnan": "error",
    "valid-typeof": "error",
    "yoda": "error"
  }
};
