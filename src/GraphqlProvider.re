open GraphqlClient;

[@bs.module "graphql-hooks"]
external clientContext: React.Context.t('props) = "ClientContext";

module ContextProvider = {
  let makeProps = (~value: generatedGraphQLClient, ~children, ()) => {
    "value": Some(value),
    "children": children,
  };
  let make = React.Context.provider(clientContext);
};

[@react.component]
let make = (~client: generatedGraphQLClient, ~children: React.element) => {
  <ContextProvider value=client> children </ContextProvider>;
};