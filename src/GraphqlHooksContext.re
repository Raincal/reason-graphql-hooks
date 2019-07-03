[@bs.module "graphql-hooks"]
external context: React.Context.t(GraphqlHooksClient.t) = "ClientContext";

module ContextProvider = {
  let makeProps = (~value: GraphqlHooksClient.t, ~children, ()) => {
    "value": value,
    "children": children,
  };
  let make = React.Context.provider(context);
};

module Provider = {
  [@react.component]
  let make = (~client: GraphqlHooksClient.t, ~children: React.element) => {
    <ContextProvider value=client> children </ContextProvider>;
  };
};

let useClient = () => {
  let client = React.useContext(context);
  client;
};