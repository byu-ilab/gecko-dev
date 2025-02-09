/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

"use strict";

const { addons, createClass, DOM: dom, PropTypes } =
  require("devtools/client/shared/vendor/react");
const { throttle } = require("devtools/client/inspector/shared/utils");

const Types = require("../types");

// The delay prior to executing the grid cell highlighting.
const GRID_CELL_MOUSEOVER_TIMEOUT = 150;

// Move SVG grid to the right 100 units, so that it is not flushed against the edge of
// layout border
const TRANSLATE_X = -100;
const TRANSLATE_Y = 0;

const VIEWPORT_HEIGHT = 100;
const VIEWPORT_WIDTH = 450;

module.exports = createClass({

  displayName: "GridOutline",

  propTypes: {
    grids: PropTypes.arrayOf(PropTypes.shape(Types.grid)).isRequired,
    onShowGridAreaHighlight: PropTypes.func.isRequired,
    onShowGridCellHighlight: PropTypes.func.isRequired,
  },

  mixins: [ addons.PureRenderMixin ],

  getInitialState() {
    return {
      selectedGrids: [],
    };
  },

  componentWillMount() {
    // Throttle the grid highlighting of grid cells. It makes the UX smoother by not
    // lagging the grid cell highlighting if a lot of grid cells are mouseover in a
    // quick succession.
    this.highlightCell = throttle(this.highlightCell, GRID_CELL_MOUSEOVER_TIMEOUT);
  },

  componentWillReceiveProps({ grids }) {
    this.setState({
      selectedGrids: grids.filter(grid => grid.highlighted),
    });
  },

  /**
   * Returns the grid area name if the given grid cell is part of a grid area, otherwise
   * null.
   *
   * @param  {Number} columnNumber
   *         The column number of the grid cell.
   * @param  {Number} rowNumber
   *         The row number of the grid cell.
   * @param  {Array} areas
   *         Array of grid areas data stored in the grid fragment.
   * @return {String} If there is a grid area return area name, otherwise null.
   */
  getGridAreaName(columnNumber, rowNumber, areas) {
    const gridArea = areas.find(area =>
      (area.rowStart <= rowNumber && area.rowEnd > rowNumber) &&
      (area.columnStart <= columnNumber && area.columnEnd > columnNumber)
    );

    if (!gridArea) {
      return null;
    }

    return gridArea.name;
  },

  highlightCell({ target }) {
    const {
      grids,
      onShowGridAreaHighlight,
      onShowGridCellHighlight,
    } = this.props;
    const name = target.getAttribute("data-grid-area-name");
    const id = target.getAttribute("data-grid-id");
    const fragmentIndex = target.getAttribute("data-grid-fragment-index");
    const color = target.getAttribute("stroke");
    const rowNumber = target.getAttribute("data-grid-row");
    const columnNumber = target.getAttribute("data-grid-column");

    target.setAttribute("fill", color);

    if (name) {
      onShowGridAreaHighlight(grids[id].nodeFront, name, color);
    }

    if (fragmentIndex && rowNumber && columnNumber) {
      onShowGridCellHighlight(grids[id].nodeFront, fragmentIndex,
        rowNumber, columnNumber);
    }
  },

  /**
   * Renders the grid outline for the given grid container object.
   *
   * @param  {Object} grid
   *         A single grid container in the document.
   */
  renderGrid(grid) {
    const { id, color, gridFragments } = grid;
    // TODO: We are drawing the first fragment since only one is currently being stored.
    // In the future we will need to iterate over all fragments of a grid.
    let gridFragmentIndex = 0;
    const { rows, cols, areas } = gridFragments[gridFragmentIndex];
    const numberOfColumns = cols.lines.length - 1;
    const numberOfRows = rows.lines.length - 1;
    const rectangles = [];

    // Draw a rectangle that acts as a border for the grid outline.
    const border = this.renderGridOutlineBorder(numberOfRows, numberOfColumns, color);
    rectangles.push(border);

    let x = 1;
    let y = 1;
    const width = 10;
    const height = 10;

    // Draw the cells within the grid outline border.
    for (let rowNumber = 1; rowNumber <= numberOfRows; rowNumber++) {
      for (let columnNumber = 1; columnNumber <= numberOfColumns; columnNumber++) {
        const gridAreaName = this.getGridAreaName(columnNumber, rowNumber, areas);
        const gridCell = this.renderGridCell(id, gridFragmentIndex, x, y, rowNumber,
          columnNumber, color, gridAreaName);

        rectangles.push(gridCell);
        x += width;
      }

      x = 1;
      y += height;
    }

    return rectangles;
  },

  /**
   * Renders the grid cell of a grid fragment.
   *
   * @param  {Number} id
   *         The grid id stored on the grid fragment
   * @param  {Number} gridFragmentIndex
   *         The index of the grid fragment rendered to the document.
   * @param  {Number} x
   *         The x-position of the grid cell.
   * @param  {Number} y
   *         The y-position of the grid cell.
   * @param  {Number} rowNumber
   *         The row number of the grid cell.
   * @param  {Number} columnNumber
   *         The column number of the grid cell.
   * @param  {String|null} gridAreaName
   *         The grid area name or null if the grid cell is not part of a grid area.
   */
  renderGridCell(id, gridFragmentIndex, x, y, rowNumber, columnNumber, color,
    gridAreaName) {
    return dom.rect(
      {
        className: "grid-outline-cell",
        "data-grid-area-name": gridAreaName,
        "data-grid-fragment-index": gridFragmentIndex,
        "data-grid-id": id,
        "data-grid-row": rowNumber,
        "data-grid-column": columnNumber,
        x,
        y,
        width: 10,
        height: 10,
        fill: "none",
        stroke: color,
        onMouseOver: this.onMouseOverCell,
        onMouseOut: this.onMouseLeaveCell,
      }
    );
  },

  renderGridOutline(grids) {
    return dom.g(
      {
        className: "grid-cell-group",
      },
      grids.map(grid => this.renderGrid(grid))
    );
  },

  renderGridOutlineBorder(numberOfRows, numberOfColumns, color) {
    return dom.rect(
      {
        className: "grid-outline-border",
        x: 1,
        y: 1,
        width: numberOfColumns * 10,
        height: numberOfRows * 10,
        stroke: color,
      }
    );
  },

  onMouseLeaveCell({ target }) {
    const {
      grids,
      onShowGridAreaHighlight,
      onShowGridCellHighlight,
    } = this.props;
    const id = target.getAttribute("data-grid-id");
    const color = target.getAttribute("stroke");

    target.setAttribute("fill", "none");

    onShowGridAreaHighlight(grids[id].nodeFront, null, color);
    onShowGridCellHighlight(grids[id].nodeFront);
  },

  onMouseOverCell(event) {
    event.persist();
    this.highlightCell(event);
  },

  render() {
    return this.state.selectedGrids.length ?
      dom.svg(
        {
          className: "grid-outline",
          width: "100%",
          height: 100,
          viewBox: `${TRANSLATE_X} ${TRANSLATE_Y} ${VIEWPORT_WIDTH} ${VIEWPORT_HEIGHT}`,
        },
        this.renderGridOutline(this.state.selectedGrids)
      )
      :
      null;
  },

});
