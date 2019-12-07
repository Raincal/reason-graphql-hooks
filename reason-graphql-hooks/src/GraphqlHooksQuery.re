type hookResponse('ret) =
  GraphqlHooksTypes.hooksResponse('ret) = {
    loading: bool,
    data: option('ret),
    cacheHit: bool,
    response: GraphqlHooksTypes.response('ret),
  };

module Make = (Config: GraphqlHooksTypes.Config) => {
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
    updateData: (Config.t, Config.t) => Config.t,
  };

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

  let use =
      (
        ~useCache=?,
        ~isManual=?,
        ~variables=?,
        ~operationName=?,
        ~skipCache=?,
        ~ssr=?,
        ~updateData=?,
        (),
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
    let state = useQueryJs(Config.query, options);

    let refetch = (~variables=?, ()) =>
      state.refetch(useQueryOptions(~variables?, ()));

    let useQueryResponseToRecord = (parse, state) => {
      let data = state.data->Js.Nullable.toOption->Belt.Option.map(parse);
      let loading = state.loading;
      let error =
        state.error->Belt.Option.map(GraphqlHooksTypes.combinedErrorToRecord);
      let cacheHit = state.cacheHit;

      let result: GraphqlHooksTypes.result('a) = {data, loading, error};

      let response: GraphqlHooksTypes.response(Config.t) =
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
        () => state |> useQueryResponseToRecord(Config.parse),
        (state, Config.parse),
      );

    (state_record, refetch);
  };
};