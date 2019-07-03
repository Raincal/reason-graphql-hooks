type t;

type fetch;

type fetchOptions =
  | FetchOpts(Fetch.requestInit)
  | FetchFn(unit => Fetch.requestInit);

let unwrapFetchOptions = fetchOptions =>
  switch (fetchOptions) {
  | FetchOpts(opts) => opts
  | FetchFn(fn) => fn()
  };

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

let make =
    (
      ~url,
      ~ssrMode=?,
      ~cache=?,
      ~fetch=?,
      ~fetchOptions=FetchOpts(Fetch.RequestInit.make()),
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
      ~fetchOptions=unwrapFetchOptions(fetchOptions),
      ~headers?,
      ~logErrors?,
      (),
    );
  client(options);
};