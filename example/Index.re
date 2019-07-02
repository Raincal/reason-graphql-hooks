let memCache = GraphqlHooksMemcache.createMemCache();

let client =
  GraphqlClient.createGraphQLClient(
    ~url="https://api.graph.cool/simple/v1/cixmkt2ul01q00122mksg82pn",
    ~cache=memCache,
    (),
  );

ReactDOMRe.renderToElementWithId(
  <GraphqlHooks.Provider client> <App /> </GraphqlHooks.Provider>,
  "app",
);