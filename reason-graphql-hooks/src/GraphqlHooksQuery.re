type hooksResponse('ret) =
  GraphqlHooksTypes.hooksResponse('ret) = {
    loading: bool,
    data: option('ret),
    cacheHit: bool,
    response: GraphqlHooksTypes.response('ret),
  };

[@bs.deriving abstract]
type useQueryOptions = {
  [@bs.optional]
  useCache: bool,
  [@bs.optional]
  isManual: bool,
  [@bs.optional]
  variables: Js.Json.t,
  [@bs.optional]
  operationName: string,
  [@bs.optional]
  skipCache: bool,
  [@bs.optional]
  ssr: bool,
  [@bs.optional]
  fetchOptionsOverrides: Fetch.requestInit,
  [@bs.optional]
  updateData: (Js.Json.t, Js.Json.t) => Js.Json.t,
};

/* [@bs.deriving abstract] */
type useQueryResponseJs = {
  loading: bool,
  cacheHit: bool,
  data: Js.Nullable.t(Js.Json.t),
  error: option(GraphqlHooksTypes.combinedErrorJs),
  refetch: useQueryOptions => Js.Promise.t(useQueryResponseJs),
};

[@bs.module "graphql-hooks"]
external useQueryJs: (string, useQueryOptions) => useQueryResponseJs =
  "useQuery";

let useQuery =
    (
      ~useCache=?,
      ~isManual=?,
      ~variables=?,
      ~operationName=?,
      ~skipCache=?,
      ~ssr=?,
      ~updateData=?,
      (parse, query, _),
    ) => {
  let options =
    useQueryOptions(
      ~useCache?,
      ~isManual?,
      ~variables?,
      ~operationName?,
      ~skipCache?,
      ~ssr?,
      ~updateData?,
      (),
    );
  let state = useQueryJs(query, options);

  let refetch = (~variables=?, ()) =>
    state.refetch(useQueryOptions(~variables?, ()));

  let useQueryResponseToReason = (parse, state) => {
    let data = state.data->Js.Nullable.toOption->Belt.Option.map(parse);
    let loading = state.loading;
    let error =
      state.error->Belt.Option.map(GraphqlHooksTypes.combinedErrorToRecord);
    let cacheHit = state.cacheHit;

    let result: GraphqlHooksTypes.result('a) = {data, loading, error};

    let response: GraphqlHooksTypes.response('data) =
      switch (result) {
      | {data: Some(data)} => Data(data)
      | {loading: true} => Loading
      | {error: Some(error)} =>
        switch (error) {
        | {graphQLErrors: Some(errors)} => Error(GraphQLErrors(errors))
        | {httpError: Some(error)} => Error(HttpError(error))
        | {fetchError: Some(error)} => Error(FetchError(error))
        | _ => NoData
        }
      | _ => NoData
      };

    {loading, data, cacheHit, response};
  };

  let state_record =
    React.useMemo2(
      () => state |> useQueryResponseToReason(parse),
      (state, parse),
    );

  (state_record, refetch);
};