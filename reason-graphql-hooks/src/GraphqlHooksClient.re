type t = {cache: GraphqlHooksMemcache.memCache};

type fetch;

[@bs.deriving abstract]
type clientOptions = {
  url: string,
  [@bs.optional]
  ssrMode: bool,
  [@bs.optional]
  cache: GraphqlHooksMemcache.memCache,
  [@bs.optional]
  logErrors: bool,
  [@bs.optional]
  fetch,
  [@bs.optional]
  fetchOptions: Fetch.requestInit,
  [@bs.optional]
  headers: Fetch.headers,
};

[@bs.module "graphql-hooks"] [@bs.new]
external client: clientOptions => t = "GraphQLClient";

[@bs.send] external setHeader: (t, string, string) => t = "setHeader";
[@bs.send] external setHeaders: (t, Fetch.headersInit) => t = "setHeaders";
[@bs.send] external removeHeader: (t, string) => t = "removeHeader";

let make =
    (
      ~url,
      ~ssrMode=?,
      ~cache=?,
      ~fetch=?,
      ~fetchOptions=?,
      ~headers=?,
      ~logErrors=?,
      (),
    ) => {
  let options =
    clientOptions(
      ~url,
      ~ssrMode?,
      ~cache?,
      ~fetch?,
      ~fetchOptions?,
      ~headers?,
      ~logErrors?,
      (),
    );
  client(options);
};