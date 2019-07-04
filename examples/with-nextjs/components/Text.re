module H1 = {
  [@react.component]
  let make = (~children) =>
    <h1 className="font-montserrat font-medium text-main-black text-3xl mb-2">
      children
    </h1>;
};

module H2 = {
  [@react.component]
  let make = (~children) =>
    <h2 className="font-montserrat font-medium text-main-black text-2xl mb-2">
      children
    </h2>;
};

module H3 = {
  [@react.component]
  let make = (~children) =>
    <h3 className="font-montserrat font-medium text-main-black text-xl mb-2">
      children
    </h3>;
};

module P = {
  [@react.component]
  let make = (~children) =>
    <p className="my-4 text-main-lighten-15"> children </p>;
};

module Ul = {
  [@react.component]
  let make = (~children) => <ul className="md-ul my-4"> children </ul>;
};

module Ol = {
  [@react.component]
  let make = (~children) =>
    <ol className="md-ol list-reset -ml-4 text-primary"> children </ol>;
};

module Li = {
  let typeOf: 'a => string = [%raw thing => "{ return typeof thing; }"];

  let isArray: 'a => bool = [%raw
    thing => "{ return thing instanceof Array; }"
  ];

  external asArray: 'a => array(ReasonReact.reactElement) = "%identity";

  let isSublist: 'a => bool = [%raw
    element => "{
        if(element == null || element.props == null) {
          return false;
        }
        const name = element.props.name;
        return name === 'ul' || name === 'ol';
      }"
  ];

  [@react.component]
  let make = (~children) => {
    /*
     There are 3 value scenarios for `children`
     1) string (if bullet point is standalone text)
     2) array(<p>, <ul>|<ol>) (if nested list)
     3) array(<p>,<inlineCode>,...,<p>) (if text with nested content)
     We are iterating on these here with quite some bailout JS
     */
    let elements: ReasonReact.reactElement =
      if (isArray(children)) {
        switch (children->asArray) {
        | [|_p, potentialSublist|] =>
          if (isSublist(potentialSublist)) {
            /* Scenario 2 */
            children;
          } else {
            <p>
               children </p>;
              /* Scenario 3 */
          }
        | _ =>
          /* Scenario 3 */
          <p> children </p>
        };
      } else if (typeOf(children) === "string") {
        <p>
           {Obj.magic(children)->ReasonReact.string} </p>;
          /* Scenario 1 */
      } else {
        /* Unknown Scenario */
        children;
      };

    <li className="md-li leading-4 text-main-lighten-15 mb-4">
      elements
    </li>;
  };
};

module A = {
  [@react.component]
  let make = (~href, ~children) =>
    <a
      href
      className="text-blue-500 hover:text-blue-700">
      children
    </a>;
};